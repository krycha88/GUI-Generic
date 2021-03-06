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

#include "../condition.h"

class OnBetweenCond : public Supla::Condition {
 public:
  OnBetweenCond(double threshold1, double threshold2, bool useAlternativeMeasurement)
      : Supla::Condition(threshold1, useAlternativeMeasurement), threshold2(threshold2) {
  }

  bool condition(double val) {
    return val > threshold && val < threshold2;
  }

  double threshold2;
};


Supla::Condition *OnBetween(double threshold1, double threshold2, bool useAlternativeMeasurement) {
  return new OnBetweenCond(threshold1, threshold2, useAlternativeMeasurement);
}



