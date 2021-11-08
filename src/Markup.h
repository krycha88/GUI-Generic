/*
  Copyright (C) krycha88

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

#ifndef Markup_h
#define Markup_h

#include "SuplaDeviceGUI.h"

void addForm(String& html, const String& method, const String& action = "/");
void addFormEnd(String& html);

void addFormHeader(String& html, const String& name = "\n");
void addFormHeaderEnd(String& html);

void addBr(String& html);
void addLabel(String& html, const String& name);

void addTextBox(String& html,
                const String& input_id,
                const String& name,
                const String& value,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false,
                bool password = false,
                bool underline = true);

void addTextBox(String& html,
                const String& input_id,
                const String& name,
                uint8_t value_key,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false,
                bool password = false);

void addTextBox(
    String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required, bool readonly = false);

void addTextBox(String& html,
                const String& input_id,
                const String& name,
                const String& value,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false);

void addTextBoxPassword(String& html, const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required);

void addCheckBox(String& html, const String& input_id, const String& name, bool checked);

void addNumberBox(String& html, const String& input_id, const String& name, uint8_t value_key, int max = -1);

void addNumberBox(String& html, const String& input_id, const String& name, const String& placeholder, bool required, const String& value = "");

void addLinkBox(String& html, const String& name, const String& url);

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr);

void addListGPIOLinkBox(String& html, const String& input_id, const String& name, const String& url, uint8_t function);

void addListGPIOBox(String& html, const String& input_id, const String& name, uint8_t function);

void addListGPIOBox(String& html,
                    const String& input_id,
                    const String& name,
                    uint8_t function,
                    uint8_t nr,
                    bool underline = true,
                    const String& url = "",
                    bool no_number = false);

void addGPIOOptionValue(String& html, uint8_t gpio, uint8_t selectedGpio, const String& name);

#ifdef SUPLA_MCP23017
void addListMCP23017GPIOBox(String& html, const String& input_id, const String& name, uint8_t function, uint8_t nr = 0, const String& url = "");
#endif

void addListBox(String& html, const String& input_id, const String& name, const char* const* list_P, uint8_t size, uint8_t selected, uint8_t nr = 0);

void addListLinkBox(String& html,
                    const String& input_id,
                    const String& name,
                    const char* const* array_P,
                    uint8_t size,
                    uint8_t selected,
                    const String& url,
                    uint8_t nr = 0);

void addButton(String& html, const String& name, const String& url);

void addButtonSubmit(String& html, const String& name);

String getURL(const String& url);

String getURL(const String& url, uint8_t nr);

String getInput(const String& input, uint8_t nr);

String getParameterRequest(const String& url, const String& param, const String& value = emptyString);

const String SuplaJavaScript(const String& java_return = PATH_START);

const String SuplaSaveResult(int save);
#endif  // Markup_h
