/** \file jma_ctype.h
 * \brief JMA_CType gives the character type information.
 * \author Vernkin
 * \version 0.1
 * \date Jun 25, 2009
 */

#ifndef JMA_CTYPE_H
#define JMA_CTYPE_H

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "knowledge.h" // Knowledge::EncodeType

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

/**
 * \brief JMA_CType gives the character type information.
 *
 * CMA_CType gives the character type information.
 */
class JMA_CType
{
public:
	typedef GenericArray<unsigned int> SepArray;

    /**
     * Create an instance of \e CMA_CType based on the character encode type.
     * \param type the character encode type
     * \return the pointer to instance
     */
    static JMA_CType* instance(Knowledge::EncodeType type);

    /**
     * Get the encoding type by the encode type string
     *
     * \param encType encode type string
     * \return assicated Knowledge::EncodeType in the class Knowledge
     */
    static Knowledge::EncodeType getEncType(string encType);

    /**
     * Destrucor
     */
    virtual ~JMA_CType();

    /**
     * Get the byte count of the first character pointed by \e p, which
     * character is in a specific encoding.
     * \param p pointer to the character string
     * \return true for punctuation, false for non punctuation.
     */
    virtual unsigned int getByteCount(const char* p) const = 0;

    /**
     * Get the number of the characters in the p
     * \param p pointer to the string to be checked
     * \return the number of the characters in the p
     */
    size_t length(const char* p) const;


    /**
     * Check whether is a seperator of sentence.
     * \param p pointer to the character string
     * \return true for separator, false for non separator.
     */
    bool isSentenceSeparator(const char* p);

    /**
     * Add the sentence separator, the duplicated one would be ignored (return false)
     * \param val the sentence separator to be add
     * \return true if the val not exists in the current sentence separators and add successfully
     */
    bool addSentenceSeparator(unsigned int val);

private:
	/**
	 * Get the number of bytes the character val occupies.
	 * \param val the character to be checked
	 * \return the number of bytes the character val occupies
	 */
	unsigned int getOccupiedBytes(unsigned int val);

private:
	/**
	 * Separator Arrays, index 0 is reserved, and index 1 ~ 4 represents
	 * character with 1 ~ 4 bytes separately.
	 */
	SepArray seps_[5];
};

} // namespace cma

#endif // JMA_CTYPE_H
