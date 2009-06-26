/*
 * generic_array.h
 *
 *  Created on: Jun 26, 2009
 *      Author: vernkin
 */

#ifndef GENERIC_ARRAY_H_
#define GENERIC_ARRAY_H_

#include <vector>

using std::vector;

namespace jma
{
/**
 * \brief GenericArray can accessing the content in the array quickly and dynamically
 * increment.
 * GenericArray can accessing the content in the array quickly and dynamically
 * increment. And the type that GenericArray stores is based on the template
 * parameter ArrayType.
 */
template<typename ArrayType>
class GenericArray{
public:

    /**
     * Create a default GenericArray with none content
     * \param pIgnDupl whether ignore duplicate elements, default is true
     */
    GenericArray(bool pIgnDupl = true)
		: data(NULL), size(0), ignDupl(pIgnDupl)
	{
    }

    /**
     * Create a GenerecArray with existent array. This method DO NOT copy the
     * the array
     * \param d existent array pointer
     * \param s existent array length
     * \param pIgnDupl whether ignore duplicate elements, default is ture
     */
    GenericArray(ArrayType* d, size_t s, bool pIgnDupl = true)
		: data(d), size(s), ignDupl(pIgnDupl)
	{
    }

    /**
     * Cpoy an instance from other GenericArray. It is deep copy.
     * \param other existent GenericArray
     */
    GenericArray(const GenericArray& other){
        size = other.length;
        if(size == 0){
            data = NULL;
            return;
        }
        data = new ArrayType[size];
        for(size_t i=0;i<size;++i)
            data[i] = other.data[i];
    }

    /**
     * Create an instance from a vector
     * \param vec outer vector
     */
    GenericArray(const vector<ArrayType>& vec){
        size = vec.size();
        data = new ArrayType[size];
        for(size_t i=0;i<size;++i){
            data[i] = vec[i];
        }
    }

    ~GenericArray(){
        if(data){
            delete []data;
            data = NULL;
        }
    }

    /**
     * Insert a new length at the end of the GenericArray
     * \param ele new element
     * \return whether insert successfully
     */
    bool insert(const ArrayType& ele)
    {
        if(size == 0) //orig is empty
        {
        	++size;
        	data = new ArrayType[size];
            data[0] = ele;
            return true;
        }

        //check whether exists
        if(ignDupl)
        {
        	if( contains(ele) )
        		return false;
        }

        ++size;
		ArrayType* orig = data;
		data = new ArrayType[size];
		size_t i = 0;
		for(; i< size - 1; ++i)
		{
			data[i] = orig[i];
		}
		data[i] = ele;
		delete []orig;
		return true;
    }

    /**
     * Whether the array is empty
     * \return true if the array is empty
     */
    bool empty()
    {
    	return size > 0;
    }

    /**
     * Whether the ele is contained in the Array
     * \param ele the element to be checked
     * \return true if the element is contained in the Array
     */
    bool contains(const ArrayType& ele)
    {
    	for( size_t i = 0 ; i < size ; ++i )
    	{
    		if( data[i] == ele )
    			return true;
    	}
    	return false;
    }

    /**
     * Store the elements
     */
    ArrayType* data;

    /**
     * The size of the elements
     */
    size_t size;

private:
    /**
     * Whether ignore Duplicate element when insert
     */
    bool ignDupl;
};
}

#endif /* GENERIC_ARRAY_H_ */
