//=============================================================================================================
/**
* @file     tmsiproducer.cpp
* @author   Lorenz Esch <lorenz.esch@tu-ilmenau.de>;
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     September, 2013
*
* @section  LICENSE
*
* Copyright (C) 2013, Lorenz Esch, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains the implementation of the TMSIProducer class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "tmsiproducer.h"
#include "tmsi.h"
#include "tmsidriver.h"

#include <QDebug>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace TMSIPlugin;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

TMSIProducer::TMSIProducer(TMSI* pTMSI)
: m_pTMSI(pTMSI)
, m_pTMSIDriver(new TMSIDriver(this))
, m_bIsRunning(true)
{
}


//*************************************************************************************************************

TMSIProducer::~TMSIProducer()
{

}


//*************************************************************************************************************

void TMSIProducer::start()
{
    //TODO: Initialise device (find, open, connect, setup buffer etc.)
    m_pTMSIDriver->InitDevice();

    m_bIsRunning = true;

    QThread::start();
}


//*************************************************************************************************************

void TMSIProducer::stop()
{
    //TODO: Uninitialise device (close device, clear buffers etc.)
    //m_pTMSIDriver->uninitiliseDevice();

    m_bIsRunning = false;
    QThread::wait();
}


//*************************************************************************************************************

void TMSIProducer::run()
{
    //unsigned int uiSamplePeriod = (unsigned int) (1000000.0/(m_pTMSI->m_fSamplingRate));
    m_bIsRunning = true;

    //
    // Inits
    //
    MatrixXf t_matRawBuffer;

    while(m_bIsRunning)
    {
        //usleep(uiSamplePeriod);

        //Get the TMSi EEG data out of the device buffer
        //t_matRawBuffer = m_pTMSIDriver->getSampleMatrixValue();

        //cout<<"... data ..."<<endl;

        //Write received data to circular buffer
        m_pTMSI->m_pRawMatrixBuffer_In->push(&t_matRawBuffer);
    }
}


