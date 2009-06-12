/** \file jma_factory.cpp
 * Implementation of class JMA_Factory.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jun 12, 2009
 */

#include "jma_factory.h"
//#include "cma_crf_factory.h"

namespace jma
{

JMA_Factory* JMA_Factory::instance_;

JMA_Factory* JMA_Factory::instance()
{
    if(instance_ == 0)
    {
        // the line below (from CMA code) is commented out,
        // this function need to be implemented in JMA.
        //instance_ = new CMA_CRF_Factory;
    }

    return instance_;
}

JMA_Factory::~JMA_Factory()
{
}

} // namespace jma
