/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _direct_link_h
#define _direct_link_h

#include <Arduino.h>
#include <WiFiClientSecure.h>

#include "../action_handler.h"
#include "../actions.h"
#include "../channel_element.h"

#define MAX_DIRECT_LINKS_SIZE 32
#define MAX_HOST_SIZE         32

namespace Supla {
enum DirectLinks {
  SEND_DIRECT_LINKS_ON,
  SEND_DIRECT_LINKS_OFF,
};
}

namespace Supla {
namespace Control {

class DirectLinks : public Element, public ActionHandler {
 public:
  DirectLinks(const char *host, bool isSecured = true);
  ~DirectLinks();

  void setHost(const char *host);
  void setUrlON(const char *url);
  void setUrlOFF(const char *url);
  void enableSSL(bool isSecured);

  void iterateAlways();
  void handleAction(int event, int action);

  bool checkConnection();
  void toggleConnection();
  bool openConnection();
  bool closeConnection();
  void sendRequest(const char *url);
  void send(const char *url);

 protected:
  WiFiClient *client = NULL;
  bool _lastStateON;
  bool _lastStateOFF;
  int _action;

  bool _isSecured;
  char _urlON[MAX_DIRECT_LINKS_SIZE];
  char _urlOFF[MAX_DIRECT_LINKS_SIZE];
  char _host[MAX_HOST_SIZE];
};

};  // namespace Control
};  // namespace Supla

#endif
