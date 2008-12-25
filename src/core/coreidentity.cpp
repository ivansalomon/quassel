/***************************************************************************
 *   Copyright (C) 2005-08 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "coreidentity.h"

#include "signalproxy.h"

CoreIdentity::CoreIdentity(IdentityId id, QObject *parent)
  : Identity(id, parent),
    _certManager(*this)
{
  connect(this, SIGNAL(idSet(IdentityId)), &_certManager, SLOT(setId(IdentityId)));
}

CoreIdentity::CoreIdentity(const Identity &other, QObject *parent)
  : Identity(other, parent),
    _certManager(*this)
{
  connect(this, SIGNAL(idSet(IdentityId)), &_certManager, SLOT(setId(IdentityId)));
}

CoreIdentity::CoreIdentity(const CoreIdentity &other, QObject *parent)
  : Identity(other, parent),
    _sslKey(other._sslKey),
    _sslCert(other._sslCert),
    _certManager(*this)
{
  connect(this, SIGNAL(idSet(IdentityId)), &_certManager, SLOT(setId(IdentityId)));
}

void CoreIdentity::synchronize(SignalProxy *proxy) {
  proxy->synchronize(this);
  proxy->synchronize(&_certManager);
}

void CoreIdentity::setSslKey(const QByteArray &encoded) {
  QSslKey key(encoded, QSsl::Rsa);
  if(key.isNull())
    key = QSslKey(encoded, QSsl::Dsa);
  setSslKey(key);
}

void CoreIdentity::setSslCert(const QByteArray &encoded) {
  setSslCert(QSslCertificate(encoded));
}

CoreIdentity &CoreIdentity::operator=(const CoreIdentity &identity) {
  Identity::operator=(identity);
  _sslKey = identity._sslKey;
  _sslCert = identity._sslCert;
  return *this;
}

// ========================================
//  CoreCertManager
// ========================================
CoreCertManager::CoreCertManager(CoreIdentity &identity)
  : CertManager(identity.id()),
    identity(identity)
{
  setAllowClientUpdates(true);
}

void CoreCertManager::setSslKey(const QByteArray &encoded) {
  identity.setSslKey(encoded);
  CertManager::setSslKey(encoded);
}

void CoreCertManager::setSslCert(const QByteArray &encoded) {
  identity.setSslCert(encoded);
  CertManager::setSslCert(encoded);
}

void CoreCertManager::setId(IdentityId id) {
  renameObject(QString::number(id.toInt()));
}