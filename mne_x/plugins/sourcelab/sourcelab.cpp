//=============================================================================================================
/**
* @file     sourcelab.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     February, 2013
*
* @section  LICENSE
*
* Copyright (C) 2013, Christoph Dinh and Matti Hamalainen. All rights reserved.
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
* @brief    Contains the implementation of the SourceLab class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "sourcelab.h"

#include <xMeas/Measurement/sngchnmeasurement.h>
#include <xMeas/Measurement/realtimesamplearray.h>
#include <xMeas/Measurement/realtimemultisamplearray_new.h>

#include "FormFiles/sourcelabsetupwidget.h"
#include "FormFiles/sourcelabrunwidget.h"


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtCore/QtPlugin>
#include <QDebug>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace SourceLabPlugin;
using namespace MNEX;
using namespace XMEASLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

SourceLab::SourceLab()
: m_pSourceLabBuffer(NULL)
{
    m_PLG_ID = PLG_ID::SOURCELAB;
}


//*************************************************************************************************************

SourceLab::~SourceLab()
{
    stop();

    if(m_pSourceLabBuffer)
        delete m_pSourceLabBuffer;
}


//*************************************************************************************************************

bool SourceLab::start()
{
    // Initialize displaying widgets
    init();

    QThread::start();
    return true;
}


//*************************************************************************************************************

bool SourceLab::stop()
{
    // Stop threads
    QThread::terminate();
    QThread::wait();

    if(m_pSourceLabBuffer)
        m_pSourceLabBuffer->clear();

    return true;
}


//*************************************************************************************************************

Type SourceLab::getType() const
{
    return _IRTAlgorithm;
}


//*************************************************************************************************************

const char* SourceLab::getName() const
{
    return "Source Lab";
}


//*************************************************************************************************************

QWidget* SourceLab::setupWidget()
{
    SourceLabSetupWidget* setupWidget = new SourceLabSetupWidget(this);//widget is later distroyed by CentralWidget - so it has to be created everytime new
    return setupWidget;
}


//*************************************************************************************************************

QWidget* SourceLab::runWidget()
{
    SourceLabRunWidget* runWidget = new SourceLabRunWidget(this);//widget is later distroyed by CentralWidget - so it has to be created everytime new
    return runWidget;
}


//*************************************************************************************************************

void SourceLab::update(Subject* pSubject)
{
    Measurement* meas = static_cast<Measurement*>(pSubject);

    //MEG
    if(!meas->isSingleChannel())
    {
        RealTimeMultiSampleArrayNew* pRTMSANew = static_cast<RealTimeMultiSampleArrayNew*>(pSubject);

        //Using fast Hash Lookup instead of if then else clause
        if(getAcceptorMeasurementBuffer(pRTMSANew->getID()))
        {
            if(pRTMSANew->getID() == MSR_ID::MEGRTSERVER_OUTPUT)
            {
                //Check if buffer initialized
                if(m_pSourceLabBuffer->size() == 0)
                {
                    mutex.lock();
                    delete m_pSourceLabBuffer;
                    m_pSourceLabBuffer = new _double_CircularMatrixBuffer(64, pRTMSANew->getNumChannels(), pRTMSANew->getMultiArraySize());
                    mutex.unlock();
                }

                MatrixXd t_mat(pRTMSANew->getNumChannels(), pRTMSANew->getMultiArraySize());

                //ToDo: Cast to specific Buffer
                for(unsigned char i = 0; i < pRTMSANew->getMultiArraySize(); ++i)
                    t_mat.col(i) = pRTMSANew->getMultiSampleArray()[i];

//                static_cast<_double_CircularMatrixBuffer*>(getAcceptorMeasurementBuffer(pRTMSANew->getID()))
//                        ->push(&t_mat);
                m_pSourceLabBuffer->push(&t_mat);

            }
        }

    }
}



//*************************************************************************************************************

void SourceLab::run()
{
    qint32 count = 0;
    while (true)
    {
        /* Dispatch the inputs */

//        double v = m_pECGBuffer->pop();

//        //ToDo: Implement here the algorithm

//        m_pSourceLab_Output->setValue(v);

        mutex.lock();
        qint32 nrows = m_pSourceLabBuffer->rows();
        mutex.unlock();

        if(nrows > 0) // check if init
        {
            MatrixXd t_mat = m_pSourceLabBuffer->pop();


            qDebug() << count << ": m_pSourceLabBuffer->pop(); Matrix:" << t_mat.rows() << "x" << t_mat.cols();

            ++count;
        }
    }
}


//*************************************************************************************************************
//=============================================================================================================
// Creating required display instances and set configurations
//=============================================================================================================

void SourceLab::init()
{
    //Delete Buffer - will be initailzed with first incoming data
    if(m_pSourceLabBuffer)
        delete m_pSourceLabBuffer;
    m_pSourceLabBuffer = new _double_CircularMatrixBuffer(0,0,0); // Init later

    qDebug() << "#### SourceLab Init; MEGRTSERVER_OUTPUT: " << MSR_ID::MEGRTSERVER_OUTPUT;

    this->addPlugin(PLG_ID::RTSERVER);
    this->addAcceptorMeasurementBuffer(MSR_ID::MEGRTSERVER_OUTPUT, m_pSourceLabBuffer);

//    m_pDummy_MSA_Output = addProviderRealTimeMultiSampleArray(MSR_ID::DUMMYTOOL_OUTPUT_II, 2);
//    m_pDummy_MSA_Output->setName("Dummy Output II");
//    m_pDummy_MSA_Output->setUnit("mV");
//    m_pDummy_MSA_Output->setMinValue(-200);
//    m_pDummy_MSA_Output->setMaxValue(360);
//    m_pDummy_MSA_Output->setSamplingRate(256.0/1.0);

}