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

void addForm(const String& method, const String& action = "/");
void addFormEnd();

void addFormHeader(const String& name = "\n");
void addFormHeader(const String& name);
void addFormHeaderEnd();

void addBr();
void addLabel(const String& name);

void addTextBox(const String& input_id,
                const String& name,
                const String& value,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false,
                bool password = false,
                bool underline = true);

void addTextBox(const String& input_id,
                const String& name,
                uint8_t value_key,
                const String& placeholder,
                int minlength,
                int maxlength,
                bool required,
                bool readonly = false,
                bool password = false);

void addTextBox(const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required, bool readonly = false);

void addTextBox(const String& input_id, const String& name, const String& value, int minlength, int maxlength, bool required, bool readonly = false);

void addTextBox(const String& value);

void addTextBoxPassword(const String& input_id, const String& name, uint8_t value_key, int minlength, int maxlength, bool required);

void addCheckBox(const String& input_id, const String& name, bool checked);

void addNumberBox(const String& input_id, const String& name, const String& value, int max = -1);

void addNumberBox(const String& input_id, const String& name, uint8_t value_key, int max = -1);

void addNumberBox(
    const String& input_id, const String& name, const String& placeholder, bool required, const String& value = "", bool underline = false);

void addLinkBox(const String& name, const String& url);

void addHyperlink(const String& name, const String& url);

void addListGPIOLinkBox(const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr);

void addListGPIOLinkBox(const String& input_id, const String& name, const String& url, uint8_t function, uint8_t nr, bool no_number);

void addListGPIOLinkBox(const String& input_id, const String& name, const String& url, uint8_t function);

void addListGPIOBox(const String& input_id, const String& name, uint8_t function);

void addListGPIOBox(
    const String& input_id, const String& name, uint8_t function, uint8_t nr, bool underline = true, const String& url = "", bool no_number = false);
void addListNumbersSensorBox(const String& input_id, const String& name, uint8_t selected);
void addListNumbersBox(const String& input_id, const String& name, uint8_t size, uint8_t selected);

void addGPIOOptionValue(uint8_t gpio, uint8_t selectedGpio, const String& name);

#ifdef GUI_SENSOR_I2C_EXPENDER
void addListExpanderBox(const String& input_id, const String& name, uint8_t function, uint8_t nr, const String& url);
void addListExpanderGPIOBox(const String& input_id, const String& name, uint8_t function, uint8_t nr = 0, const String& url = "");
void addListExpanderGPIO(
    const String& input_id, const String& name, uint8_t function, uint8_t nr, const char* const* array_P, uint8_t size, const String& url);
#endif

void addListBox(
    const String& input_id, const String& name, const char* const* list_P, uint8_t size, uint8_t selected, uint8_t nr = 0, bool underline = true);

void addListLinkBox(
    const String& input_id, const String& name, const char* const* array_P, uint8_t size, uint8_t selected, const String& url, uint8_t nr = 0);

void addButton(const String& name, const String& url);
void addButton(const String& name, const String& url);

void addButtonSubmit(const String& name);

String getURL(const String& url);

String getURL(const String& url, uint8_t nr);

String getInput(const String& input, uint8_t nr);

String getParameterRequest(const String& url, const String& param, const String& value = emptyString);

void SuplaJavaScript(const String& java_return = PATH_START);

void SuplaSaveResult(int save);

#endif  // Markup_h
