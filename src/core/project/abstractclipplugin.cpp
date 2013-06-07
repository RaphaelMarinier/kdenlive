/*
Copyright (C) 2012  Till Theato <root@ttill.de>
This file is part of Kdenlive. See www.kdenlive.org.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
*/

#include "abstractclipplugin.h"
#include "clippluginmanager.h"
#include "abstracttimelineclip.h"
#include "timelineview/timelineclipitem.h"


AbstractClipPlugin::AbstractClipPlugin(ClipPluginManager* parent) :
    QObject(parent),
    m_parent(parent)
{

}

AbstractClipPlugin::~AbstractClipPlugin()
{
}

TimelineClipItem* AbstractClipPlugin::timelineClipView(AbstractTimelineClip *clip, QGraphicsItem *parent) const
{
    return new TimelineClipItem(clip, parent);
}

#include "abstractclipplugin.moc"
