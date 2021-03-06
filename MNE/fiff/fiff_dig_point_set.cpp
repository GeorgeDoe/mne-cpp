//=============================================================================================================
/**
* @file     fiff_dig_point_set.cpp
* @author   Jana Kiesel <jana.kiesel@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2016
*
* @section  LICENSE
*
* Copyright (C) 2016, Jana Kiesel and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    fiff_dig_point_set class definition.
*
*/


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "fiff_dig_point_set.h"


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "fiff_dig_point.h"
#include "fiff_dir_tree.h"
#include "fiff_tag.h"
#include "fiff_types.h"


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace FIFFLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE GLOBAL METHODS
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

FiffDigPointSet::FiffDigPointSet()
{
}


//*************************************************************************************************************

FiffDigPointSet::FiffDigPointSet(const FiffDigPointSet &p_FiffDigPointSet)
: m_qListDigPoint(p_FiffDigPointSet.m_qListDigPoint)
{

}


//*************************************************************************************************************

FiffDigPointSet::FiffDigPointSet(QIODevice &p_IODevice)   //const FiffDigPointSet &p_FiffDigPointSet
{
    //
    //   Open the file
    //
    FiffStream::SPtr t_pStream(new FiffStream(&p_IODevice));
    FiffDirTree t_Tree;

    if(!FiffDigPointSet::readFromStream(t_pStream, t_Tree, *this))
    {
        t_pStream->device()->close();
        qDebug() << "Could not read the FiffDigPointSet\n"; // ToDo throw error
    }
}


//*************************************************************************************************************

FiffDigPointSet::~FiffDigPointSet()
{

}


//*************************************************************************************************************

bool FiffDigPointSet::readFromStream(FiffStream::SPtr &p_pStream, FiffDirTree &p_Tree, FiffDigPointSet &p_Dig)
{
    //
    //   Open the file, create directory
    //
    bool open_here = false;

    if (!p_pStream->device()->isOpen())
    {
        QList<FiffDirEntry> t_Dir;
        QString t_sFileName = p_pStream->streamName();

        if(!p_pStream->open(p_Tree, t_Dir))
        {
            return false;
        }
        printf("Opening header data %s...\n",t_sFileName.toUtf8().constData());

        open_here = true;
//        if(t_pDir)
//            delete t_pDir;
    }

    //
    //   Read the measurement info
    //
    //read_hpi_info(p_pStream,p_Tree, info);
    fiff_int_t kind = -1;
    fiff_int_t pos = -1;
    FiffTag::SPtr t_pTag;

    //
    //   Locate the Electrodes
    //
    QList<FiffDirTree> isotrak = p_Tree.dir_tree_find(FIFFB_ISOTRAK);

    fiff_int_t coord_frame = FIFFV_COORD_HEAD;
    FiffCoordTrans dig_trans;
    qint32 k = 0;

    if (isotrak.size() == 1)
    {
        for (k = 0; k < isotrak[0].nent; ++k)
        {
            kind = isotrak[0].dir[k].kind;
            pos  = isotrak[0].dir[k].pos;
            if (kind == FIFF_DIG_POINT)
            {
                FiffTag::read_tag(p_pStream.data(), t_pTag, pos);
                p_Dig.m_qListDigPoint.append(t_pTag->toDigPoint());
            }
            else
            {
                if (kind == FIFF_MNE_COORD_FRAME)
                {
                    FiffTag::read_tag(p_pStream.data(), t_pTag, pos);
                    qDebug() << "NEEDS To BE DEBBUGED: FIFF_MNE_COORD_FRAME" << t_pTag->getType();
                    coord_frame = *t_pTag->toInt();
                }
                else if (kind == FIFF_COORD_TRANS)
                {
                    FiffTag::read_tag(p_pStream.data(), t_pTag, pos);
                    qDebug() << "NEEDS To BE DEBBUGED: FIFF_COORD_TRANS" << t_pTag->getType();
                    dig_trans = t_pTag->toCoordTrans();
                }
            }
        }
    }
    for(k = 0; k < p_Dig.size(); ++k)
    {
        p_Dig[k].coord_frame = coord_frame;
    }

    //
    //   All kinds of auxliary stuff
    //
    if(open_here)
    {
        p_pStream->device()->close();
    }
    return true;
}


//*************************************************************************************************************

const FiffDigPoint& FiffDigPointSet::operator[] (qint32 idx) const
{
    if (idx>=m_qListDigPoint.length())
    {
        qWarning("Warning: Required DigPoint doesn't exist! Returning DigPoint '0'.");
        idx=0;
    }
    return m_qListDigPoint[idx];
}


//*************************************************************************************************************

FiffDigPoint& FiffDigPointSet::operator[] (qint32 idx)
{
    if (idx >= m_qListDigPoint.length())
    {
        qWarning("Warning: Required DigPoint doesn't exist! Returning DigPoint '0'.");
        idx = 0;
    }
    return m_qListDigPoint[idx];
}


//*************************************************************************************************************

FiffDigPointSet &FiffDigPointSet::operator<<(const FiffDigPoint &dig)
{
    this->m_qListDigPoint.append(dig);
    return *this;
}


//*************************************************************************************************************

FiffDigPointSet &FiffDigPointSet::operator<<(const FiffDigPoint *dig)
{
    this->m_qListDigPoint.append(*dig);
    return *this;
}
