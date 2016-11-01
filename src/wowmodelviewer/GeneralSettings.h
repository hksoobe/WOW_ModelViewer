/*
 * GeneralSettings.h
 *
 *  Created on: 1 may 2015
 *      Author: Jeromnimo
 */

#ifndef _GENERALSETTINGS_H_
#define _GENERALSETTINGS_H_

// WX Headers
#include <wx/wxprec.h>
#include <wx/filepicker.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

enum
{
  ID_SETTINGS_RANDOMSKIN,
  ID_SETTINGS_SHOWPARTICLE,
  ID_SETTINGS_ZEROPARTICLE,
  ID_SETTINGS_DISPLAYIDINLIST,
  ID_GENERAL_SETTINGS_APPLY,
  ID_FIND_GAME_FOLDER,
  ID_FIND_CUSTOM_FOLDER,
  ID_ERASE_CUSTOM_FOLDER,
};

enum {
  CHECK_RANDOMSKIN,
  CHECK_SHOWPARTICLE,
  CHECK_ZEROPARTICLE,
  CHECK_DISPLAYIDINLIST,

  NUM_SETTINGS1_CHECK
};

class GeneralSettings: public wxWindow
{
  DECLARE_CLASS(GeneralSettings)
    DECLARE_EVENT_TABLE()

  wxCheckBox *chkbox[NUM_SETTINGS1_CHECK];
  wxTextCtrl *gamePathDisplay;
  wxTextCtrl *customDirectoryPathDisplay;
  wxRadioBox *keepPolicyRadioBox;
  wxString newGamePath, newCustomFolder;
public:
  GeneralSettings(wxWindow* parent, wxWindowID id);
  ~GeneralSettings(){};

  void Update();

  void OnButton(wxCommandEvent &event);
  void OnCheck(wxCommandEvent &event);
};



#endif /* _GENERALSETTINGS_H_ */
