/*
 * Copyright 2013-2015  Christian Dávid <christian-david@web.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "onlinejobfolder.h"

onlineJobFolder::onlineJobFolder() :
    m_folder(folderOutbox)
{
}

onlineJobFolder::onlineJobFolder(const onlineJobFolders& folder) :
    m_folder(folder)
{
}

onlineJobFolder::onlineJobFolder(const onlineJobFolder &other)
    : m_folder(other.m_folder)
{
}

onlineJobFolder onlineJobFolder::outbox()
{
  return onlineJobFolder(folderOutbox);
}

onlineJobFolder onlineJobFolder::drafts()
{
  return onlineJobFolder(folderDrafts);
}

onlineJobFolder onlineJobFolder::templates()
{
  return onlineJobFolder(folderTemplates);
}

onlineJobFolder onlineJobFolder::historic()
{
  return onlineJobFolder(folderHistoric);
}
