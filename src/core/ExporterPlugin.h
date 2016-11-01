/*----------------------------------------------------------------------*\
| This file is part of WoW Model Viewer                                  |
|                                                                        |
| WoW Model Viewer is free software: you can redistribute it and/or      |
| modify it under the terms of the GNU General Public License as         |
| published by the Free Software Foundation, either version 3 of the     |
| License, or (at your option) any later version.                        |
|                                                                        |
| WoW Model Viewer is distributed in the hope that it will be useful,    |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with WoW Model Viewer.                                           |
| If not, see <http://www.gnu.org/licenses/>.                            |
\*----------------------------------------------------------------------*/

/*
 * ImporterPlugin.h
 *
 *  Created on: 17 Feb. 2015
 *   Copyright: 2015 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#ifndef _EXPORTERPLUGIN_H_
#define _EXPORTERPLUGIN_H_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL
#include <string>
#include <vector>

// Qt
#include <QObject>

// Externals
#include "GL/glew.h"

// Other libraries

// Current library
class Model;
#include "Plugin.h"

// Namespaces used
//--------------------------------------------------------------------


// Class Declaration
//--------------------------------------------------------------------
#ifdef _WIN32
#    ifdef BUILDING_CORE_DLL
#        define _EXPORTERPLUGIN_API_ __declspec(dllexport)
#    else
#        define _EXPORTERPLUGIN_API_ __declspec(dllimport)
#    endif
#else
#    define _EXPORTERPLUGIN_API_
#endif

class _EXPORTERPLUGIN_API_ ExporterPlugin : public Plugin
{
  public :
    // Constants / Enums

    // Constructors
    ExporterPlugin() : m_canExportAnimation(false) {}

    // Destructors

    // Methods
    virtual std::string menuLabel() const = 0;
    virtual std::string fileSaveTitle() const = 0;
    virtual std::string fileSaveFilter() const = 0;

    virtual bool exportModel(Model *, std::string file) = 0;

    bool canExportAnimation() const { return m_canExportAnimation; }

    void setAnimationsToExport(std::vector<int> values) { m_animsToExport = values; }

    // Members

  protected :
    // Constants / Enums

    // Constructors

    // Destructors

    // Methods
    void exportGLTexture(GLuint id, std::string filename) const;

    // Members
    bool m_canExportAnimation;
    std::vector<int> m_animsToExport;

  private :
    // Constants / Enums

    // Constructors

    // Destructors

    // Methods

    // Members

    // friend class declarations

};

// static members definition
#ifdef _EXPORTERPLUGIN_CPP_
Q_DECLARE_INTERFACE(ExporterPlugin,"wowmodelviewer.exporterplugin/1.0");

#endif

#endif /* _EXPORTERPLUGIN_H_ */
