/***************************************************************************
                          transitiondialog.cpp  -  description
                             -------------------
    begin                :  Mar 2006
    copyright            : (C) 2006 by Jean-Baptiste Mardelle
    email                : jb@ader.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <cmath>

#include <qnamespace.h>
#include <qhgroupbox.h>
#include <qhbox.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qslider.h>
#include <qcheckbox.h>

#include <kpushbutton.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

#include "transition.h"
#include "docclipref.h"
#include "transitiondialog.h"


namespace Gui {

    TransitionDialog::TransitionDialog(KdenliveApp * app, QWidget * parent,
                                       const char *name): KJanusWidget(parent, name, IconList), m_transition(0)
{
    //QTabWidget
    QFont dialogFont = font();
    dialogFont.setPointSize(dialogFont.pointSize() - 1);
    setFont(dialogFont);
    transitCrossfade = new transitionCrossfade_UI(addPage(i18n("Crossfade"), QString::null, KGlobal::iconLoader()->loadIcon("kdenlive_trans_down", KIcon::Small, 15)));

    transitWipe = new transitionWipeWidget(addPage(i18n("Wipe"), QString::null, KGlobal::iconLoader()->loadIcon("kdenlive_trans_wiper", KIcon::Small, 15)));

    transitPip = new transitionPipWidget(app, 240,192,addPage(i18n("PIP"), QString::null, KGlobal::iconLoader()->loadIcon("kdenlive_trans_pip", KIcon::Small, 15)));

    /*transitAudiofade = new transitionAudiofade_UI(addPage(i18n("Audio Fade"), QString::null, KGlobal::iconLoader()->loadIcon("kdenlive_trans_down", KIcon::Small, 15)));*/

    setEnabled(false);
    adjustSize();
}

TransitionDialog::~TransitionDialog() 
{
    disconnectTransition();
    delete transitCrossfade;
    delete transitWipe;
    delete transitPip;
}

bool TransitionDialog::isOnTrack(int ix)
{
    if (m_transition && m_transition->transitionDocumentTrack() == ix) return true;
    return false;
}

void TransitionDialog::setTransition(Transition *transition)
{
        disconnectTransition();
	m_transition = transition;
        if (transition == 0) {
            setEnabled(false);
            return;
        }
        setEnabled(true);
	setActivePage(transition->transitionType());
        setTransitionDirection(transition->invertTransition());
        setTransitionParameters(transition->transitionParameters());
        connectTransition();
}


void TransitionDialog::connectTransition()
{
   connect(this, SIGNAL( aboutToShowPage ( QWidget * )), this, SLOT(applyChanges()));
   connect(transitWipe, SIGNAL(applyChanges ()), this, SLOT(applyChanges()));
   connect(transitCrossfade->invertTransition, SIGNAL(released()), this, SLOT(applyChanges()));
   
   connect(transitPip, SIGNAL(transitionChanged()), this, SLOT(applyChanges()));
}

void TransitionDialog::disconnectTransition()
{
    disconnect(this, SIGNAL( aboutToShowPage ( QWidget * )), this, SLOT(applyChanges()));

    disconnect(transitWipe, SIGNAL(applyChanges ()), this, SLOT(applyChanges()));
    disconnect(transitCrossfade->invertTransition, SIGNAL(released()), this, SLOT(applyChanges()));

    disconnect(transitPip, SIGNAL(transitionChanged()), this, SLOT(applyChanges()));
}

bool TransitionDialog::isActiveTransition(Transition *transition)
{
	if (transition == m_transition) return true;
	return false;
}

bool TransitionDialog::belongsToClip(DocClipRef *clip)
{
	if (m_transition && m_transition->belongsToClip(clip)) return true;
	return false;
}


void TransitionDialog::applyChanges()
{
	if (m_transition) {
		if (m_transition == 0) return;
		kdDebug()<<"* * * * * TRANS: "<<m_transition->transitionName() <<endl;
		m_transition->setTransitionType(selectedTransition());
        	m_transition->setTransitionParameters(transitionParameters());
		m_transition->setTransitionDirection(transitionDirection());
		emit transitionChanged(true);
        }
}

void TransitionDialog::setActivePage(const Transition::TRANSITIONTYPE &pageName)
{
    switch (pageName) {
    case Transition::COMPOSITE_TRANSITION:
	showPage(1);
	break;
    case Transition::PIP_TRANSITION:
	showPage(2);
	break;
    case Transition::MIX_TRANSITION:
	showPage(3);
	break;
    default:
	showPage(0);
	break;
    }
}

Transition::TRANSITIONTYPE TransitionDialog::selectedTransition() 
{
    switch (activePageIndex()) {
	case 1: 
	return Transition::COMPOSITE_TRANSITION;
	break;
	case 2: 
	return Transition::PIP_TRANSITION;
	break;
	case 3: 
	return Transition::MIX_TRANSITION;
	break;
	default: 
	return Transition::LUMA_TRANSITION;
	break;	
    }
}

void TransitionDialog::setTransitionDirection(bool direc)
{
    transitCrossfade->invertTransition->setChecked(direc);
    //transitAudiofade->invertTransition->setChecked(direc);

}

void TransitionDialog::setTransitionParameters(const QMap < QString, QString > parameters)
{
    if (activePageIndex() == 1) {
        transitWipe->rescaleImages->setChecked(parameters["distort"].toInt());
	transitWipe->setParameters(parameters["geometry"]);
    }
    else if (activePageIndex() == 2) {
        transitPip->setParameters(parameters["geometry"]);
        }
}

bool TransitionDialog::transitionDirection()
{
    bool result = true;
    if (activePageIndex() == 0) result = transitCrossfade->invertTransition->isChecked();
    else if (activePageIndex() == 1) result = transitWipe->invertTransition->isChecked();
    //else if (activePageIndex() == 3) result = transitAudiofade->invertTransition->isChecked();
    //if (activePageIndex() == 2) result = transitPip->invertTransition->isChecked();
    return result;
}

        
const QMap < QString, QString > TransitionDialog::transitionParameters() 
{
    QMap < QString, QString > paramList;
    if (activePageIndex() == 0) return paramList; // crossfade
    if (activePageIndex() == 1) // wipe
    {
	return transitWipe->parameters();
    }
    else if (activePageIndex() == 2) // pip
    {
      paramList["geometry"] = transitPip->parameters();
      paramList["progressive"] = "1";
    }
    else if (activePageIndex() == 3) // audio mix
    {
      paramList["start"] = "0";
      paramList["end"] = "1";
      paramList["combine"] = "1";
    }
    
    return paramList;
}

} // namespace Gui


