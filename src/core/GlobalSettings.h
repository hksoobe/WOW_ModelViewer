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
 * GlobalSettings.h
 *
 *  Created on: 26 nov. 2011
 *   Copyright: 2013 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#ifndef _GLOBALSETTINGS_H_
#define _GLOBALSETTINGS_H_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL
#include <string>

// Wx

// Externals

// Other libraries

// Current library


// Namespaces used
//--------------------------------------------------------------------


// Class Declaration
//--------------------------------------------------------------------
#ifdef _WIN32
#    ifdef BUILDING_CORE_DLL
#        define _GLOBALSETTINGS_API_ __declspec(dllexport)
#    else
#        define _GLOBALSETTINGS_API_ __declspec(dllimport)
#    endif
#else
#    define _GLOBALSETTINGS_API_
#endif

#define GLOBALSETTINGS GlobalSettings::instance()

class _GLOBALSETTINGS_API_ GlobalSettings
{
  public :
    // Constants / Enums

    // Constructors

    // Destructors
    ~GlobalSettings();

    // Methods
    static GlobalSettings & instance()
    {
      if(GlobalSettings::m_instance == 0)
        GlobalSettings::m_instance = new GlobalSettings();

      return *m_instance;
    }

    std::string appVersion(std::string a_prefix = std::string(""));
    std::string appName();
    std::string buildName();
    std::string appTitle();

    bool isBeta() { return m_isBetaVersion; }

    // Members
    // find a better way than a lot of members...
    bool bShowParticle;
    bool bZeroParticle;
    bool bInitPoseOnlyExport;

  protected :
    // Constants / Enums

    // Constructors

    // Destructors

    // Methods

    // Members

  private :
    // Constants / Enums

    // Constructors
    GlobalSettings();
    GlobalSettings(GlobalSettings &);

    // Destructors

    // Methods

    // Members

    int m_versionMajorNumber;
    int m_versionMinorNumber;
    int m_versionRevNumber;
    std::string m_versionSpecialExtend;

    std::string	m_appName;
    std::string	m_buildName;
    std::string m_platform;

    bool m_isBetaVersion;
    bool m_isAlphaVersion;

    static GlobalSettings * m_instance;

    // friend class declarations
};

// static members definition
#ifdef _GLOBALSETTINGS_CPP_

#endif

#endif /* _GLOBALSETTINGS_H_ */
