/*
||
|| @file AverageList.h
|| @version 1.2
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Provide an easy way of averaging values
|| #
||
|| @license
|| |  This library is free software; you can redistribute it and/or
|| |  modify it under the terms of the GNU Lesser General Public
|| |  License as published by the Free Software Foundation; version
|| |  2.1 of the License.
|| |
|| |  This library is distributed in the hope that it will be useful,
|| |  but WITHOUT ANY WARRANTY; without even the implied warranty of
|| |  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
|| |  Lesser General Public License for more details.
|| |
|| |  You should have received a copy of the GNU Lesser General Public
|| |  License along with this library; if not, write to the Free Software
|| |  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
|| #
||
*/

#ifndef AverageList_h
#define AverageList_h

#include<Arduino.h>
#include<Print.h>


template<typename datatype>
class AverageList {
	public:
		//<<constructor>>
		AverageList(datatype* storage, byte storageSize) : values(storage) , size(storageSize) { 
			currentSize = 0; 
			currentIndex = 0; 

		}
		
		//add a value to the AverageList at the next index, if out of bounds, return false, else return true
		void addValue(datatype value){ 
			currentIndex=currentIndex+1;
			if (currentSize<size){
				values[currentSize++] = value;
				currentIndex = currentIndex % currentSize;
			}else{
				currentIndex = currentIndex % size;
				values[currentIndex] = value; 
			}
		}

		//return the average of all values added with an addValue call
		datatype getAverage(){
			float buffer = 0;
			for (byte i=0; i<currentSize; i++){ buffer += (float)values[i]; }
			buffer /= currentSize;
			return (datatype)buffer;
		}


		/*
		 * return the average of all circular (0-360) values in DEGREES added with an addRotationalValue call
		 * Range is 0-360 DEGREES
		 */
		datatype getRotationalAverage(){
			float x = 0;
			float y = 0;
			float angle=0;

			for (byte i=0; i<currentSize; i++){
				angle=TWO_PI*values[i];
			    x += cos(angle);
			    y += sin(angle);

			}
			//TODO:watch out for zeros
			if(x==0.0f && y==0.0f){
				return 0.0f;
			}
			return degrees(atan2(y, x));

		}

		//return the average of the entire AverageList, even though all values might not be initialized
		datatype getTotalAverage(){
			datatype buffer = 0;
			for (byte i=0; i<size; i++){ buffer += values[i]; }
			buffer /= size;
			return buffer;
		}
		
		//return the low peak of  the values in the set
		datatype getMax(){
			datatype max=values[0];
			for (byte i=1; i<size; i++){ 
				if (values[i]>max){
					max = values[i];
					currentIndex = i;
				}
			}
			return max;
		}
		
		//return the low peak of  the values in the set
		datatype getMin(){
			datatype min=values[0];
			for (byte i=1; i<size; i++){ 
				if (values[i]<min){
					min = values[i];
					currentIndex = i;
				}
			}
			return min;
		}
		
		//reset the AverageList
		void reset(){ 
			currentSize = 0; 
			currentIndex = 0;
			for (byte i=0; i<size; i++){ values[i]=0; }
		}
		
		//reset and assign a value
		AverageList& operator=(const datatype& rhs){
			reset();
			addValue(rhs);
			return *this;
		}
		
		//add a value to the AverageList
		AverageList& operator+=(const datatype& rhs){
			addValue(rhs);
			return *this;
		}
		
		//remove a value from the AverageList if not existant, do nothing
		AverageList& operator-=(const datatype& rhs){
			remove(rhs);
			return *this;
		}
		
		//subscript operator
		datatype& operator[]( int subscript ){
			return values[ subscript<0 ? 0 : subscript>size ? size-1 : subscript ];
		}
		//subscript operator on const AverageList objects
		datatype operator[]( int subscript ) const{
			return values[ subscript<0 ? 0 : subscript>size ? size-1 : subscript ];
		}
		//remove a value from the AverageList if not existant, do nothing
		bool remove(const datatype& rhs){
			for (byte i=0; i<currentSize; i++){ 
                if (values[i]==rhs){
					currentIndex = i;
					if(i+1<currentSize){
    					do{
    						values[i]=values[i+1];
    						i++;
    					}while(i+1<currentSize);
                	}
					currentSize--;
					return true;
				}
			}
			return false;
		}
		//return the current index
		byte getCurrentIndex() { return currentIndex; }
		byte getCurrentSize() { return currentSize; }
		datatype getValue(byte x) { return values[x]; }
		void debug(char* name,Print& printer){
			printer.print(name);
			printer.print(" [");
			for (byte i=0; i<size; i++){
				sizeof(datatype)==1 ? printer.print(values[i],DEC) : printer.print(values[i]);
				if (i+1<size){
					printer.print(", ");
				}
			}
			printer.println("]");
		}

	private:
		datatype* values;	//pointer to allocated space
		byte size;			//keep track of the size of the values array
		byte currentSize;	//keep track of the current index
		byte currentIndex;	//keep track of the current index


};
#endif

/*
|| @changelog
|| | 1.2 2009-07-04 - Alexander Brevig : Changed Lisence
|| |                                     Added remove()
|| |                                     Added print()
|| | 1.1 2009-05-21 - Alexander Brevig : Added getMax and getMin, @request http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?action=viewprofile;username=retrolefty
|| | 1.0 2009-05-21 - Alexander Brevig : Initial Release
|| #
*/

