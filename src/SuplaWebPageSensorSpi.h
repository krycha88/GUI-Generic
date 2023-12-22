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

#ifndef SuplaWebPageSensorSpi_h
#define SuplaWebPageSensorSpi_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_MAX6675) || defined(SUPLA_MAX31855) || defined (SUPLA_CC1101)
#define GUI_SENSOR_SPI
#endif

#ifdef GUI_SENSOR_SPI

#define PATH_SPI "spi"

#define INPUT_MAX6675  "max6675"
#define INPUT_MAX31855 "imax31855"
#define INPUT_CLK_GPIO "clk"
#define INPUT_CS_GPIO  "cs"
#define INPUT_MISO_GPIO  "d0" // miso
#define INPUT_MOSI_GPIO "mosi"


#ifdef SUPLA_CC1101
#define INPUT_CC1101 "cc1101"
#define INPUT_GDO0_GPIO  "gdo0"
#define INPUT_GDO2_GPIO  "gdo2"
#define INPUT_WMBUS_SENSOR_TYPE "s_type"
#define INPUT_WMBUS_SENSOR_ID "s_number"
#define INPUT_WMBUS_SENSOR_KEY "s_key"
#define INPUT_WMBUS_SENSOR_PROP "s_prop"
const char* const sensors_types[] /*PROGMEM*/ = {
    "izar", 
    "amiplus", 
    "apator08", 
    "apator162", 
    "apatoreitn", 
    "bmeters",
    "c5isf", 
    "compact5",
    "dme07",
    "elf",
    "evo868",
    "fhkvdataiii",
    "hydrocalm3",
    "hydrodigit",
    "hydrus",
    "iperl",
    "itron",
    "izar",
    "mkradio3",
    "mkradio4",
    "qheat",
    "qwater",
    "sharky774",
    "topaseskr",
    "ultrimis",
    "unismart",
    "vario451"
  };

enum _wmbus_config_positions
{
  WMBUS_CFG_SENSOR_TYPE,
  WMBUS_CFG_SENSOR_PROPERTY
};
#endif

void createWebPageSensorSpi();
void handleSensorSpi(int save = 0);
void handleSensorSpiSave();
#endif

#endif  // ifndef SuplaWebPageSensorSpi_h
