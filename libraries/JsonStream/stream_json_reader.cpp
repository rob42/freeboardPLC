/*
 * dinamic_string.cpp
 *
 *  Created on: 07/03/2014
 *      Author: laso
 */
/*  Copied from http://blog.biicode.com/arduino-parse-json/
 *  No licence mentioned on site - attempting to contact owner
 *  Mods to avoid cstringlibs
 *
 */

#include "stream_json_reader.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//using namespace std;

namespace stream_json_reader {

	StreamJsonReader::StreamJsonReader(HardwareSerial* serial,
									  const char* queries[],
									  unsigned int num_queries,
									  unsigned int max_name_size,
									  unsigned int max_value_size,
									  unsigned int max_trace_size) : queries((char**)queries),
																	 num_queries(num_queries),
																	 max_value_size(max_value_size),
																	 max_trace_size(max_trace_size),
																	 max_name_size(max_name_size),
																	 realloc_increment(5),
																	 found_results(0),
																	 ignore_node(false){

		this->serial=serial;
		alloc_elements();
	}

	StreamJsonReader::~StreamJsonReader(){
		free_elements();
	}

	void StreamJsonReader::free_elements(){
		free(this->element_value);
		free(this->element_name);
		free(this->trace);
		for(int i=0; i<num_queries; i++){
			free(this->results[i]);
		}
		free(this->results);
	}

	void StreamJsonReader::alloc_elements(){
		this->trace = (char*) malloc(max_trace_size * sizeof(char));
		this->element_name = (char*) malloc(max_name_size * sizeof(char));
		this->element_value = (char*) malloc(max_value_size * sizeof(char));


		status = NODE_VALUE;
		element_type = TYPE_DICT;
		element_value[0] = '\0';
		element_name[0] = '\0';
		trace[0] = '\0';

		//Alloc memory for result pointers
		results = (char**) malloc(num_queries * sizeof(char*));

		//Init results with a \0
		for(int i=0; i<num_queries; i++){
			results[i] = (char*) malloc(1 * sizeof(char));
			results[i][0] = '\0';
		}
	}

	void StreamJsonReader::reset(){
		free_elements();
		found_results = 0;
		status = NODE_VALUE;
		alloc_elements();
	}

	// MATCH PARTIAL

	void StreamJsonReader::assign_result(char* result){
		//Check trace against queries and assign result
		for(int i=0; i < num_queries; i++){
			if(query_match(queries[i], trace)){
				results[i] = (char*) realloc(results[i], (strlen(result)+1)*sizeof(char));
				str_copy(results[i], result);
				found_results++;
				return;
			}
		}
	}

	bool StreamJsonReader::finished(){
		return found_results == num_queries;
	}

	bool StreamJsonReader::query_match(char* query, char* trace){
		if(strlen(trace) != strlen(query)){
			            return false;
			        }
			        for(unsigned int i=0; i < strlen(trace); i++){ //dont compare \0
			            if(query[i] != trace[i]){
			                return false;
			            }
			        }
			        return true;
	}


	bool StreamJsonReader::partial_query_match(char* trace){
		//query starts with trace, we need to keep looking on this node
		for(int i=0; i < num_queries; i++){
			if(starts_with(queries[i], trace)){
				return true;
			}
		}
		return false;
	}



	bool StreamJsonReader::is_numeric_last_trace_element(){
		int size = strlen(trace);
		if(size==0){
			return false;
		}
		for(int i=size-1; i >= 0; i--){
			if(trace[i] == '.'){
				return true;
			}
			if(!(trace[i] >= '0' && trace[i] <= '9')){
				return false;
			}
		}
		return true;
	}

	int StreamJsonReader::numeric_last_trace_element_value(){
		//cosa.1  => 1
		int size = strlen(trace);
		int value = 0;
		if(!is_numeric_last_trace_element()){
			return -1;
		}
		for(int i=size-1; i >= 0; i--){
			if(trace[i] != '.'){
				value += atoi(trace[i]) * pow(10, (size-1-i));
			}
			else{
				return value;
			}
		}
		return -1;
	}


	void StreamJsonReader::increment_trace_element(){
		//cosa.1 to cosa.2
		int value = numeric_last_trace_element_value();
		if(value == -1){
			return;
		}
		int size = strlen(trace);
		char as_string[5]; // limited to 5 numbers indexes
		int_to_string(value + 1, as_string);

		for(unsigned int i=size-1; i >= 0; i--){
			if(trace[i] == '.'){
				for(unsigned int j=0; j <= strlen(as_string); j++){ //copy \0 too
					if(i+j+2 < max_trace_size){
						trace[i+1+j] = as_string[j];
					}
				}
				return;
			}
		}
		return;
	}

	void StreamJsonReader::remove_last_trace_element(){
		int size = strlen(trace);
		int i;
		for(i=size-1; i >= 0; i--){
			if(trace[i] == '.'){
				trace[i] = '\0';
				ignore_node = !partial_query_match(trace);
				return;
			}
		}
		trace[0] = '\0';
		ignore_node = !partial_query_match(trace);

	}

	void StreamJsonReader::add_to_trace(char* element){

		if(strlen(element) == 0){
			return;
		}
		int size = strlen(trace);
		if(size > 0){
			if(!append_to_trace('.')){
				return;
			}
			size++;
		}
		for(unsigned int i=0; i < strlen(element); i++){ //copy \0
			append_to_trace(element[i]);
		}
		ignore_node = !partial_query_match(trace);
	}

	int StreamJsonReader::process_char(char c){
		serial->print(F("Process char, status:"));
		serial->print(status);
		serial->print(F(", element type:"));
		serial->print(element_type);
		serial->print(F(", char:"));
		serial->println(c);
		switch(status){
			case NODE_VALUE: //At the beggining is a node value of root. Wait for { or [ or number or "
				switch(c){
					case '{': status = READ_ELEMENT; element_type = TYPE_DICT; break;
					case '[': status = NODE_VALUE; add_to_trace((char*) "0"); break;
					case '"': status = READ_ELEMENT; element_type = TYPE_STRING; element_value[0] = '\0'; break;
					case ']': remove_last_trace_element(); status = WAIT_FOR_ANOTHER_ELEMENT_OR_CLOSE; break; //Empty array
					default:
						if((c >= '0' && c <= '9') || c == '-'){
							 status = READ_ELEMENT;
							 element_type = TYPE_NUMERIC;
							 append_to_value(c); //must be cleared before here
						}
						else{
							return -1;
						}
						break;
				}
				break;
			case READ_ELEMENT: //We are reading an element of type element_type

				switch(element_type){
					case TYPE_DICT:
						// Inside a dict we only expect a element name
						if(c == '}'){
							add_to_trace((char*) "0");
							status = WAIT_FOR_ANOTHER_ELEMENT_OR_CLOSE;
							return process_char(c);
						}
						if(c != '"') return -1;
						status = READING_ELEMENT_KEY;
						element_name[0] = '\0';
						break;
					case TYPE_STRING:
						// Inside a string, concat until "
						if(c != '"'){
							append_to_value(c);

						}
						else{
							//cout << "*PUT STRING! " << element_value << " ==> " << trace << endl;
							serial->print(F("*PUT STRING! "));
														serial->print(element_value);
															serial->print(F(", trace="));
																serial->println(trace);
							assign_result(element_value);
							element_value[0] = '\0';
							status = WAIT_FOR_ANOTHER_ELEMENT_OR_CLOSE;
						}
						break;
					case TYPE_NUMERIC:
						// Inside a numeric, concat until } or ,
						if(c != '}' && c != ',' && c != ']') append_to_value(c);
						else{
							// FIN VALOR DE NUMERO!!! DEVOLVERLO!!
							serial->print(F("*PUT NUMERO! "));
							serial->print(element_value);
								serial->print(F(", trace="));
									serial->println(trace);
							assign_result(element_value);
							element_value[0] = '\0';
							status = WAIT_FOR_ANOTHER_ELEMENT_OR_CLOSE;
							return process_char(c); // Advance one status
						}
						break;
				}//end switch
				break;
			case WAIT_FOR_ANOTHER_ELEMENT_OR_CLOSE:

				if(c == '}' || c == ']'){
					status = WAIT_FOR_ANOTHER_ELEMENT_OR_CLOSE;
					remove_last_trace_element(); // parent value ended
					if(is_numeric_last_trace_element()){
						//element_type = TYPE_ARRAY;
					}
					else{
						element_type = TYPE_DICT;
					}
				}
				else if(c == ','){
					if(!is_numeric_last_trace_element()){
						status = READ_ELEMENT;
						remove_last_trace_element(); // value with key ended
						element_type = TYPE_DICT;
					}
					else{
						status = NODE_VALUE; // Any type of value
						increment_trace_element();
						//element_type = TYPE_ARRAY;
					}
				}else{
					//end of json?
					status = NODE_VALUE;
				}

				break;
			case READING_ELEMENT_KEY:
				if(c != '"'){
					append_to_name(c);
				}
				else{
					status = WAIT_FOR_TWO_POINTS; // !!!!!! READ FOR MATCH NAME!!
					add_to_trace(element_name);
					element_name[0] = '\0';
				}
				break;

			case WAIT_FOR_TWO_POINTS:
				if(c != ':') return -1;
				else status = NODE_VALUE;
				break;
		}

		return 0;
	}

	bool StreamJsonReader::append_to_trace(char c){
		unsigned int size = strlen(this->trace);
		if(size + 1 > this->max_trace_size){
			this->trace = resize_string(this->trace, &this->max_trace_size); //Modifies string pointer if realloc moves on memory
		}
		if(!append_to_string(this->trace, c, this->max_trace_size)){
			return false;
		}
		return true;
	}

	bool StreamJsonReader::append_to_value(char c){
		unsigned int size = strlen(this->element_value);
		if(size + 1 > this->max_value_size){
			this->element_value = resize_string(this->element_value, &this->max_value_size); //Modifies string pointer if realloc moves on memory
		}
		if(!append_to_string(this->element_value, c, this->max_value_size)){
			return false;
		}
		return true;
	}

	bool StreamJsonReader::append_to_name(char c){
		unsigned int size = strlen(this->element_name);
		if(size + 1 > this->max_name_size){
			this->element_name = resize_string(this->element_name, &this->max_name_size); //Modifies string pointer if realloc moves on memory
		}
		if(!append_to_string(this->element_name, c, this->max_name_size)){
			return false;
		}
		return true;
	}


	unsigned int* StreamJsonReader::get_string_max_size(char* string){
		unsigned int *max_size;
		if(string==this->element_name){
			max_size = &this->max_name_size;
		}
		else if(string==this->element_value){
			max_size = &this->max_value_size;
		}
		else if(string==this->trace){
			max_size = &this->max_trace_size;
		}
		return max_size;
	}


	char* StreamJsonReader::resize_string(char* string, unsigned int* max_size){
		size_t next_size = (size_t) (*max_size + realloc_increment) * sizeof(char);

		char* tmp = (char*) realloc(string, next_size);
		if(tmp == NULL){
			return string;
		}
		else{
			*max_size += realloc_increment;
			return tmp;
		}
	}


	int atoi(char c){
		return c - '0';
	}


	 void StreamJsonReader::int_to_string(int value, char* buffer){
	        sprintf(buffer, "%d", value);
	    }

	    void StreamJsonReader::str_copy(char *dest, const char* source){
	        for(unsigned int i=0; i <= strlen(source); i++){
	           dest[i] = source[i];
	        }
	    }

	    int StreamJsonReader::len(const char* string){
	      int i=0;
	       while(string[i] != '\0'){
	           i++;
	       }
	       //serial->print("String = ");
	       //serial->print(string);
	       //serial->print(",len=");
	       //serial->println(i);
	       //return strlen(string);
	        return i;
	    }



	    bool StreamJsonReader::starts_with(const char* string, const char* other){
	        //String begins with other
	        if(strlen(other) > strlen(string)){
	            return false;
	        }
	        for(unsigned int i=0; i < strlen(other); i++){ //dont compare \0
	            if(string[i] != other[i]){
	                return false;
	            }
	        }
	        return true;
	    }

	    bool StreamJsonReader::append_to_string(char* string, const char c, int max_size){
	        int size = strlen(string);
	        if(size + 1 > max_size){
	            return false; //Can't append more
	        }
	        string[size] = c;
	        string[size + 1] = '\0';
	        return true;
	    }

} /* namespace weather_reader */
