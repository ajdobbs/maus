/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>

#include "TCanvas.h"

#include "src/common_cpp/DataStructure/CanvasWrapper.hh"

namespace MAUS {
CanvasWrapper::CanvasWrapper() : _description(""), _canvas(NULL) {
}

CanvasWrapper::CanvasWrapper(const CanvasWrapper& data) : _canvas(NULL) {
    *this = data;
}

CanvasWrapper& CanvasWrapper::operator=(const CanvasWrapper& rhs) {
    if (this == &rhs)
        return *this;
    _description = rhs._description;
    if (_canvas != NULL)
        delete _canvas;
    if (rhs._canvas != NULL) {
        _canvas = new TCanvas();
        _canvas->cd(); 
        dynamic_cast<TCanvas*>(rhs._canvas->DrawClone());
        _canvas->SetTitle(rhs._canvas->GetTitle());
    } else {
        _canvas = NULL;
    }
    return *this;
}

CanvasWrapper::~CanvasWrapper() {
    if (_canvas != NULL)
        delete _canvas;
}

void CanvasWrapper::SetCanvas(TCanvas* canvas) {
    if (_canvas != NULL)
        delete _canvas;
    _canvas = canvas;
}
} // namespace MAUS

