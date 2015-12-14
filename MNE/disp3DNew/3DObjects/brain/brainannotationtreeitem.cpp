//=============================================================================================================
/**
* @file     brainannotationtreeitem.cpp
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     November, 2015
*
* @section  LICENSE
*
* Copyright (C) 2015, Lorenz Esch and Matti Hamalainen. All rights reserved.
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
* @brief    BrainAnnotationTreeItem class definition.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "brainannotationtreeitem.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISP3DNEWLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

BrainAnnotationTreeItem::BrainAnnotationTreeItem(const int &iType, const QString &text)
: AbstractTreeItem(iType, text)
{
}


//*************************************************************************************************************

BrainAnnotationTreeItem::~BrainAnnotationTreeItem()
{
}


//*************************************************************************************************************

QVariant BrainAnnotationTreeItem::data(int role) const
{
    switch(role) {
        case BrainAnnotationTreeItemRoles::AnnotColors:
            return QVariant();
    }

    return QStandardItem::data(role);
}


//*************************************************************************************************************

void  BrainAnnotationTreeItem::setData(const QVariant &value, int role)
{
    QStandardItem::setData(value, role);
}


//*************************************************************************************************************

bool BrainAnnotationTreeItem::addData(const Surface &tSurface, const Annotation &tAnnotation)
{
    //Create color from annotation data if annotation is not empty
    if(tAnnotation.getVertices().rows() != 0) {
        Matrix<float, Dynamic, 3, RowMajor> matColorsAnnot(tAnnotation.getVertices().rows(), 3);
        QList<FSLIB::Label> qListLabels;
        QList<RowVector4i> qListLabelRGBAs;

        tAnnotation.toLabels(tSurface, qListLabels, qListLabelRGBAs);

        for(int i = 0; i<qListLabels.size(); i++) {
            FSLIB::Label label = qListLabels.at(i);
            for(int j = 0; j<label.vertices.rows(); j++) {
                matColorsAnnot(label.vertices(j), 0) = qListLabelRGBAs.at(i)(0)/255.0;
                matColorsAnnot(label.vertices(j), 1) = qListLabelRGBAs.at(i)(1)/255.0;
                matColorsAnnot(label.vertices(j), 2) = qListLabelRGBAs.at(i)(2)/255.0;
            }
        }
    }

    //Add annotation meta information
    BrainTreeItem *itemShowAnnot = new BrainTreeItem(BrainTreeModelItemTypes::AnnotToggleVisibility, "Show");
    itemShowAnnot->setCheckable(true);
    *this<<itemShowAnnot;

    BrainTreeItem *itemAnnotFileName = new BrainTreeItem(BrainTreeModelItemTypes::AnnotFileName, tAnnotation.fileName());
    *this<<itemAnnotFileName;

    BrainTreeItem *itemAnnotPath = new BrainTreeItem(BrainTreeModelItemTypes::AnnotFilePath, tAnnotation.filePath());
    *this<<itemAnnotPath;

    return true;
}
