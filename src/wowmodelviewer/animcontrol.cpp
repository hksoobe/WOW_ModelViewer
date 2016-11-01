#include "animcontrol.h"

#include <wx/wx.h>
#include <algorithm>
#include <wx/combobox.h>
#include "logger/Logger.h"
#include "FileTreeItem.h"
#include "Game.h"
#include "GameDatabase.h"
#include "globalvars.h"
#include "modelviewer.h"
#include "UserSkins.h"
#include "util.h"
#include "WMOGroup.h"


IMPLEMENT_CLASS(AnimControl, wxWindow)

BEGIN_EVENT_TABLE(AnimControl, wxWindow)
  EVT_COMBOBOX(ID_ANIM, AnimControl::OnAnim)
  EVT_COMBOBOX(ID_ANIM_SECONDARY, AnimControl::OnAnim)
  EVT_TEXT_ENTER(ID_ANIM_SECONDARY_TEXT, AnimControl::OnButton)
  EVT_COMBOBOX(ID_ANIM_MOUTH, AnimControl::OnAnim)

  EVT_COMBOBOX(ID_LOOPS, AnimControl::OnLoop)
  EVT_COMBOBOX(ID_SKIN, AnimControl::OnSkin)
  EVT_COMBOBOX(ID_ITEMSET, AnimControl::OnItemSet)

  EVT_COMBOBOX(ID_BLP_SKIN1, AnimControl::OnBLPSkin)
  EVT_COMBOBOX(ID_BLP_SKIN2, AnimControl::OnBLPSkin)
  EVT_COMBOBOX(ID_BLP_SKIN3, AnimControl::OnBLPSkin)

  EVT_CHECKBOX(ID_OLDSTYLE, AnimControl::OnCheck)
  EVT_CHECKBOX(ID_ANIM_LOCK, AnimControl::OnCheck)
  EVT_CHECKBOX(ID_ANIM_NEXT, AnimControl::OnCheck)

  EVT_BUTTON(ID_SHOW_BLP_SKINLIST, AnimControl::OnButton)
  EVT_BUTTON(ID_PLAY, AnimControl::OnButton)
  EVT_BUTTON(ID_PAUSE, AnimControl::OnButton)
  EVT_BUTTON(ID_STOP, AnimControl::OnButton)
  EVT_BUTTON(ID_ADDANIM, AnimControl::OnButton)
  EVT_BUTTON(ID_CLEARANIM, AnimControl::OnButton)
  EVT_BUTTON(ID_PREVANIM, AnimControl::OnButton)
  EVT_BUTTON(ID_NEXTANIM, AnimControl::OnButton)

  EVT_SLIDER(ID_SPEED, AnimControl::OnSliderUpdate)
  EVT_SLIDER(ID_SPEED_MOUTH, AnimControl::OnSliderUpdate)
  EVT_SLIDER(ID_FRAME, AnimControl::OnSliderUpdate)
END_EVENT_TABLE()

AnimControl::AnimControl(wxWindow* parent, wxWindowID id)
{
  LOG_INFO << "Creating Anim Control...";

  if(Create(parent, id, wxDefaultPosition, wxSize(700,120), 0, wxT("AnimControlFrame")) == false)
  {
    wxMessageBox(wxT("Failed to create a window for our AnimControl!"), wxT("Error"));
    LOG_ERROR << "Failed to create a window for our AnimControl!";
    return;
  }

  const wxString strLoops[10] = { wxT("0"), wxT("1"), wxT("2"), wxT("3"), wxT("4"),
                                  wxT("5"), wxT("6"), wxT("7"), wxT("8"), wxT("9")};
	
  animCList = new wxComboBox(this, ID_ANIM, _("Animation"), wxPoint(10,10), wxSize(150,16), 0,
                             NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, wxT("Animation"));
  animCList2 = new wxComboBox(this, ID_ANIM_SECONDARY, _("Secondary"), wxPoint(10,95), wxSize(150,16), 0,
                             NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, wxT("Secondary"));
  animCList2->Enable(false);
  animCList2->Show(false);

  lockText = new wxTextCtrl(this, ID_ANIM_SECONDARY_TEXT, wxEmptyString, wxPoint(300, 64),
                            wxSize(20, 20), wxTE_PROCESS_ENTER, wxDefaultValidator);
  lockText->SetValue(wxString::Format(wxT("%d"), UPPER_BODY_BONES));
  lockText->Enable(false);
  lockText->Show(false);

  // Our hidden head/mouth related controls
  animCList3 = new wxComboBox(this, ID_ANIM_MOUTH, _("Mouth"), wxPoint(170,95), wxSize(150,16), 0,
                              NULL, wxCB_READONLY|wxCB_SORT, wxDefaultValidator, wxT("Secondary"));
  animCList3->Enable(false);
  animCList3->Show(false);

  //btnPauseMouth = new wxButton(this, ID_PAUSE_MOUTH, wxT("Pause"), wxPoint(160,100), wxSize(45,20));
  //btnPauseMouth->Show(false);

  speedMouthLabel = new wxStaticText(this, -1, wxT("Speed: 1.0x"), wxPoint(340,95), wxDefaultSize);
  speedMouthLabel->Show(false);

  speedMouthSlider = new wxSlider(this, ID_SPEED_MOUTH, 10, 0, 40, wxPoint(415,95), wxSize(100,38), wxSL_AUTOTICKS);
  speedMouthSlider->SetTickFreq(10, 1);
  speedMouthSlider->Show(false);

  // ---

  loopList = new wxComboBox(this, ID_LOOPS, wxT("0"), wxPoint(330, 10), wxSize(40,16), 10,
                            strLoops, wxCB_READONLY, wxDefaultValidator, wxT("Loops"));
  btnAdd = new wxButton(this, ID_ADDANIM, _("Add"), wxPoint(380, 10), wxSize(45,20));

  skinList = new wxComboBox(this, ID_SKIN, _("Skin"), wxPoint(170,10), wxSize(150,16), 0, NULL, wxCB_READONLY);
  skinList->Show(false);

  BLPSkinsLabel = new wxStaticText(this, wxID_ANY, wxT("All skins in folder :"), wxPoint(600,5), wxSize(150,16));
  BLPSkinsLabel->Show(false);

  showBLPList = new wxButton(this, ID_SHOW_BLP_SKINLIST, _("Show skin list (LONG!)"), wxPoint(635,25), wxSize(150,22));
  showBLPList->Show(false);

  BLPSkinLabel1 = new wxStaticText(this, wxID_ANY, wxT("Skin 1"), wxPoint(600,29), wxSize(30,16));
  BLPSkinLabel1->Show(false);
  BLPSkinList1 = new wxComboBox(this, ID_BLP_SKIN1, _("Skin"), wxPoint(635,25), wxSize(150,16), 0, NULL, wxCB_READONLY);
  BLPSkinList1->Show(false);

  BLPSkinLabel2 = new wxStaticText(this, wxID_ANY, wxT("Skin 2"), wxPoint(600,59), wxSize(30,16));
  BLPSkinLabel2->Show(false);
  BLPSkinList2 = new wxComboBox(this, ID_BLP_SKIN2, _("Skin"), wxPoint(635,55), wxSize(150,16), 0, NULL, wxCB_READONLY);
  BLPSkinList2->Show(false);

  BLPSkinLabel3 = new wxStaticText(this, wxID_ANY, wxT("Skin 3"), wxPoint(600,89), wxSize(30,16));
  BLPSkinLabel3->Show(false);
  BLPSkinList3 = new wxComboBox(this, ID_BLP_SKIN3, _("Skin"), wxPoint(635,85), wxSize(150,16), 0, NULL, wxCB_READONLY);
  BLPSkinList3->Show(false);

  randomSkins = true;
  defaultDoodads = true;
  modelFolderChanged = true;
  BLPListFilled = false;

  wmoList = new wxComboBox(this, ID_ITEMSET, _("Item set"), wxPoint(220,10), wxSize(128,16), 0, NULL, wxCB_READONLY);
  wmoList->Show(FALSE);
  wmoLabel = new wxStaticText(this, -1, wxEmptyString, wxPoint(10,15), wxSize(192,16));
  wmoLabel->Show(FALSE);

  speedSlider = new wxSlider(this, ID_SPEED, 10, 1, 40, wxPoint(490,56), wxSize(100,38), wxSL_AUTOTICKS);
  speedSlider->SetTickFreq(10, 1);
  speedLabel = new wxStaticText(this, -1, wxT("Speed: 1.0x"), wxPoint(490,40), wxDefaultSize);

  frameLabel = new wxStaticText(this, -1, wxT("Frame: 0"), wxPoint(330,40), wxDefaultSize);
  frameSlider = new wxSlider(this, ID_FRAME, 1, 1, 10, wxPoint(330,56), wxSize(160,38), wxSL_AUTOTICKS);
  frameSlider->SetTickFreq(2, 1);

  btnPlay = new wxButton(this, ID_PLAY, _("Play"), wxPoint(10,40), wxSize(45,20));
  btnPause = new wxButton(this, ID_PAUSE, _("Pause"), wxPoint(62,40), wxSize(45,20));
  btnStop = new wxButton(this, ID_STOP, _("Stop"), wxPoint(115,40), wxSize(45,20));
	
  btnClear = new wxButton(this, ID_CLEARANIM, _("Clear"), wxPoint(10,64), wxSize(45,20));
  btnPrev = new wxButton(this, ID_PREVANIM, wxT("<<"), wxPoint(62,64), wxSize(45,20));
  btnNext = new wxButton(this, ID_NEXTANIM, wxT(">>"), wxPoint(115,64), wxSize(45,20));
	
  lockAnims = new wxCheckBox(this, ID_ANIM_LOCK, _("Lock Animations"), wxPoint(170,64), wxDefaultSize, 0);
  bLockAnims = true;
  lockAnims->SetValue(bLockAnims);

  oldStyle = new wxCheckBox(this, ID_OLDSTYLE, _("Auto Animate"), wxPoint(170,40), wxDefaultSize, 0);
  bOldStyle = true;
  oldStyle->SetValue(bOldStyle);
  nextAnims = new wxCheckBox(this, ID_ANIM_NEXT, _("Next Animations"), wxPoint(430,10), wxDefaultSize, 0);
  bNextAnims = false;
  nextAnims->SetValue(bNextAnims);
}

AnimControl::~AnimControl()
{
  // Free the memory that was allocated (fixed: memory leak)
  for (size_t i=0; i<skinList->GetCount(); i++)
  {
    TextureGroup *grp = (TextureGroup *)skinList->GetClientData((unsigned int)i);
    wxDELETE(grp);
  }

  animCList->Clear();
  animCList2->Clear();
  animCList3->Clear();
  skinList->Clear();
  BLPSkinList1->Clear();
  BLPSkinList2->Clear();
  BLPSkinList3->Clear();

  animCList->Destroy();
  animCList2->Destroy();
  animCList3->Destroy();
  skinList->Destroy();
  BLPSkinList1->Destroy();
  BLPSkinList2->Destroy();
  BLPSkinList3->Destroy();
  BLPSkinsLabel->Destroy();
  BLPSkinLabel1->Destroy();
  BLPSkinLabel2->Destroy();
  BLPSkinLabel3->Destroy();
  PCRList = std::vector<particleColorReplacements>();
}

void AnimControl::UpdateModel(WoWModel *m)
{
  if (!m)
    return;
  PCRList.clear();
  CDIToTexGp.clear();
  // Clear skin/texture data from previous model - if there is any.
  if (g_selModel)
  {
    for (size_t i=0; i<skinList->GetCount(); i++)
    {
      TextureGroup *grp = (TextureGroup *)skinList->GetClientData((unsigned int)i);
      wxDELETE(grp);
    }
  }
  // --

  LOG_INFO << "Update model:" << m->itemName();

  g_selModel = m;

  selectedAnim = 0;
  selectedAnim2 = -1;
  selectedAnim3 = -1;

  animCList->Clear();
  animCList2->Clear();
  animCList3->Clear();

  skinList->Clear();

  QString modelpath = GetModelFolder(m);
  if (modelFolder != modelpath)  // new model is in different folder to old
  {
    modelFolderChanged = true;
    BLPListFilled = false;
    modelFolder = modelpath;
    BLPskins.clear();
    BLPSkinList1->Clear();
    BLPSkinList2->Clear();
    BLPSkinList3->Clear();
  }
  else
  {
    modelFolderChanged = false;
    BLPSkinList1->SetSelection(wxNOT_FOUND);
    BLPSkinList2->SetSelection(wxNOT_FOUND);
    BLPSkinList3->SetSelection(wxNOT_FOUND);
  }

  showBLPList->Show(false);
  BLPSkinList1->Show(false);
  BLPSkinList2->Show(false);
  BLPSkinList3->Show(false);
  BLPSkinsLabel->Show(false);
  BLPSkinLabel1->Show(false);
  BLPSkinLabel2->Show(false);
  BLPSkinLabel3->Show(false);

  ssize_t useanim = -1;

  // Find any textures that exist for the model
  bool res = false;

  wxString fn = m->itemName().toStdString().c_str();
  fn = fn.Lower();

  if (fn.substr(0,4) != wxT("char"))
  {
    if (fn.substr(0,8) == wxT("creature"))
    {
      res = UpdateCreatureModel(m);
    }
    else if (fn.substr(0,4) == wxT("item"))
    {
      res = UpdateItemModel(m);
    }
  }

  skinList->Show(res);

  // A small attempt at keeping the 'previous' animation that was selected when changing
  // the selected model via the model control.
  /*
	// Alfred 2009.07.19 keep currentAnim may crash others if it doesn't have, we should save the animID, not currentAnim
	if (g_selModel->currentAnim > 0)
		useanim = g_selModel->currentAnim;
   */

  /*
	if (g_selModel->charModelDetails.isChar) { // only display the "secondary" animation list if its a character
		animCList2->Select(useanim);
		animCList2->Show(true);
		lockAnims->Show(true);
		loopList->Show(true);
		btnAdd->Show(true);
	} else {
		animCList2->Show(false);
		lockAnims->Show(true);
		loopList->Show(false);
		btnAdd->Show(false);
	}
   */

  // Animation stuff
  if (m->animated && m->anims)
  {
    wxString strName;
    wxString strStand;
    int selectAnim = 0;

    map<int, string> animsVal = m->getAnimsMap();

    for (size_t i=0; i<m->header.nAnimations; i++)
    {
      std::stringstream label;
      label << animsVal[m->anims[i].animID];
      label << " [";
      label << i;
      label << "]";
      wxString strName=label.str().c_str();

      if (g_selModel->anims[i].animID == ANIM_STAND && useanim == -1)
      {
        strStand = strName;
        useanim = i;
      }

      animCList->Append(strName);
      animCList2->Append(strName);
      animCList3->Append(strName);
    }

    if (useanim != -1)
    {
      for(unsigned int i=0; i<animCList->GetCount(); i++)
      {
        strName = animCList->GetString(i);
        if (strName == strStand)
        {
          selectAnim = i;
          break;
        }
      }
    }

    if (useanim==-1)
      useanim = 0;
    //return;

    g_selModel->currentAnim = useanim; // anim position in anims
    animCList->Select(selectAnim); // anim position in selection
    animCList->Show(true);

    frameSlider->SetRange(g_selModel->anims[useanim].timeStart, g_selModel->anims[useanim].timeEnd);
    frameSlider->SetTickFreq(g_selModel->anims[useanim].playSpeed, 1);

    g_selModel->animManager->SetAnim(0, useanim, 0);
    if (bNextAnims && g_selModel)
    {
      int NextAnimation = useanim;
      for(size_t i=1; i<4; i++)
      {
        NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
        if (NextAnimation >= 0)
          g_selModel->animManager->AddAnim(NextAnimation, loopList->GetSelection());
        else
          break;
      }
    }
    g_selModel->animManager->Play();
  }
  wmoList->Show(false);
  wmoLabel->Show(false);
}

void AnimControl::UpdateWMO(WMO *w, int group)
{
	if (!w || w->itemName().size()==0)
		return;

	bool newwmo = (oldname != w->itemName().toStdString());
	oldname = w->itemName().toStdString();

	//Model *m = static_cast<Model*>(canvas->root->children[0]);

	//if (!m || m->anims==NULL)
	//	return;

	//m->animManager->Reset();
	g_selWMO = w;


	frameSlider->SetRange(0, 10);
	frameSlider->SetTickFreq(2, 1);
	PCRList.clear();
	animCList->Show(false);
	skinList->Show(false);
	showBLPList->Show(false);
	BLPSkinList1->Show(false);
	BLPSkinList2->Show(false);
	BLPSkinList3->Show(false);
	BLPSkinsLabel->Show(false);
	BLPSkinLabel1->Show(false);
	BLPSkinLabel2->Show(false);
	BLPSkinLabel3->Show(false);

	loopList->Show(false);
	btnAdd->Show(false);
	
	if (newwmo) {
		// build itemset list
		wmoList->Clear();
		wmoList->Append(wxT("(No doodads)"));

		for (size_t i=0; i<g_selWMO->doodadsets.size(); i++) {
			wmoList->Append(wxString(g_selWMO->doodadsets[i].name, *wxConvCurrent));
		}

		int sel = defaultDoodads ? 1 : 0;
		g_selWMO->includeDefaultDoodads = defaultDoodads;
		wmoList->Select(sel);
		g_selWMO->showDoodadSet(sel-1);
	}
	wmoList->Show(TRUE);

	// get wmo name or current wmogroup name/descr
	if (group>=-1 && group<(int)g_selWMO->nGroups) {
		wxString label = w->itemName().toStdString().c_str();
		label = label.AfterLast(MPQ_SLASH);
		if (group>=0) {
			label += wxT(" - ") + g_selWMO->groups[group].name;
			if (g_selWMO->groups[group].desc.length()) {
				label += wxT(" - ") + g_selWMO->groups[group].desc;
			}
		}
		wmoLabel->SetLabel(label);
	} else {
		wmoLabel->SetLabel(wxT("This group has been removed from the WMO"));
	}
	wmoLabel->Show(TRUE);
}

void AnimControl::SetSkinByDisplayID(int cdi)
{
  if (!cdi)
    return;

  if (CDIToTexGp.find(cdi) == CDIToTexGp.end())
  {
    LOG_INFO << "AnimControl::SetSkinByDisplayID : DisplayInfo ID (" << cdi << ") not associated with this model";
    return;
  }

  TextureGroup gp = CDIToTexGp[cdi];
  // Find position of texture group in menu and set the model to this appearance:
  int count = skinList->GetCount();
  for (int i = 0; i < count; i++)
  {
    // note that these aren't the same group, just equivalent:
    if (gp == *(static_cast<TextureGroup *> (skinList->GetClientData(i))))
    {
      SetSkin(i);
      return;
    }
  }
  LOG_ERROR << "No matching texture group found for cdi " << cdi;
}

QString AnimControl::GetModelFolder(WoWModel *m)
{
  return QString(m->itemName().toStdString().c_str()).section(MPQ_SLASH, 0, -2) + MPQ_SLASH;
}

Vec4D AnimControl::fromARGB(int color)
{
  const float alpha = ((color & 0xFF000000) >> 24) / 255.0f;
  const float red = ((color & 0x00FF0000) >> 16) / 255.0f;
  const float green = ((color & 0x0000FF00) >>  8) / 255.0f;
  const float blue = ((color & 0x000000FF)      ) / 255.0f;
  return Vec4D(red, green, blue, alpha);
// Note: the above alpha is probably irrelevant. It doesn't seem to be included. We always set the particle to its default one
}

bool AnimControl::UpdateCreatureModel(WoWModel *m)
{
  int numPCRs = 0;

  std::set<GameFile *> alreadyUsedTextures;
  TextureSet skins;

  // see if this model has skins
  LOG_INFO << "Searching skins for" << m->itemName();

  wxString fn = m->itemName().toStdString().c_str();
  fn.MakeLower();

  wxString modelname = fn.AfterLast(SLASH);

  // remove extension
  fn = fn.BeforeLast(wxT('.'));

  QString query = QString("SELECT Texture1, Texture2, Texture3, FileData.path, ParticleColorID, "
                          "CreatureDisplayInfo.ID FROM CreatureDisplayInfo "
                          "LEFT JOIN CreatureModelData ON CreatureDisplayInfo.ModelID = CreatureModelData.ID "
                          "LEFT JOIN FileData ON CreatureModelData.FileDataID = FileData.ID "
                          "WHERE FileData.name = \"%1\" COLLATE NOCASE")
                          .arg( modelname.c_str());

  sqlResult r = GAMEDATABASE.sqlQuery(query);
  PCRList.clear();
  if(r.valid && !r.values.empty())
  {
    for(size_t i = 0 ; i < r.values.size() ; i++)
    {
      TextureGroup grp;
      int count = 0;
      for (size_t skin = 0; skin < TextureGroup::num; skin++)
      {
        if(!r.values[i][skin].isEmpty())
        {
          GameFile * tex = GAMEDIRECTORY.getFile(r.values[i][3] + r.values[i][skin] + ".blp");
          alreadyUsedTextures.insert(tex);
          grp.tex[skin] = tex;
          count++;
        }
      }
      int cdi = r.values[i][5].toInt();
      grp.base = TEXTURE_GAMEOBJECT1;
      grp.definedTexture = true;
      grp.count = count;
      int pci = r.values[i][4].toInt(); // particleColorIndex, for replacing particle color
      if (pci)
      {
        grp.particleColInd = pci;
        QString pciquery = QString("SELECT StartColor1, MidColor1, EndColor1, "
	      "StartColor2, MidColor2, EndColor2, StartColor3, MidColor3, EndColor3 FROM ParticleColor "
	      "WHERE ID = %1;").arg(pci);
        sqlResult pcir = GAMEDATABASE.sqlQuery(pciquery);
        if(pcir.valid && !pcir.empty())
        {
          std::vector<Vec4D> cols;
          for (size_t j = 0; j < pcir.values[0].size(); j++)
          {
            cols.push_back(fromARGB(pcir.values[0][j].toInt()));
          }
          PCRList.push_back({ {cols[0],cols[1],cols[2]}, {cols[3],cols[4],cols[5]}, {cols[6],cols[7],cols[8]} });
          grp.PCRIndex = numPCRs;
          numPCRs++;
        }
      }
      else
          grp.PCRIndex = -1;

      if(grp.tex[0] != 0 && std::find(skins.begin(), skins.end(), grp) == skins.end())
        skins.insert(grp);
      CDIToTexGp[cdi] = grp;
    }
  }

  int count = (int)skins.size();

  LOG_INFO << "Found" << skins.size() << "skins (Database)";

  // Search the model's directory for all BLPs:
  std::vector<GameFile *> folderFiles;

  // get all files in the model's folder:
  GAMEDIRECTORY.getFilesForFolder(folderFiles, modelFolder, ".blp");
  // Add folder textures to main skin list only if :
  // - We didn't find any texures in the displayInfo database
  // - Or we found textures but the model only requires 1 texture at a time
  // (Models will look bad if they require multiple textures but only one is set)
  int numConfigSkins = m->canSetTextureFromFile(TEXTURE_GAMEOBJECT1) +
                       m->canSetTextureFromFile(TEXTURE_GAMEOBJECT2) +
                       m->canSetTextureFromFile(TEXTURE_GAMEOBJECT3);
  if (folderFiles.begin() != folderFiles.end())
  {
    for (std::vector<GameFile *>::iterator it = folderFiles.begin(); it != folderFiles.end(); ++it)
    {
      TextureGroup grp;
      grp.base = TEXTURE_GAMEOBJECT1;
      grp.count = 1;
      GameFile * tex = *it;
      grp.tex[0] =  tex;
      grp.definedTexture = false;
      if (modelFolderChanged)
        BLPskins.insert(grp);
      // append to main list only if not already included as part of database-defined textures:
      if ((numConfigSkins == 1) &&
          (std::find(alreadyUsedTextures.begin(), alreadyUsedTextures.end(), tex) ==
           alreadyUsedTextures.end()) &&
          (std::find(skins.begin(), skins.end(), grp) == skins.end()))
        skins.insert(grp);
    }
  }

  if ((!BLPListFilled || modelFolderChanged) && !BLPskins.empty())
    FillBLPSkinSelector(BLPskins);

  // Creatures can have 1-3 textures that can be taken from game files.
  // But it varies from model to model which of the three textures can
  // be set this way.
  if (m->canSetTextureFromFile(TEXTURE_GAMEOBJECT1))
  {
    BLPSkinList1->Show(true);
    BLPSkinLabel1->Show(true);
    BLPSkinsLabel->Show(true);
  }
  if (m->canSetTextureFromFile(TEXTURE_GAMEOBJECT2))
  {
    BLPSkinList2->Show(true);
    BLPSkinLabel2->Show(true);
    BLPSkinsLabel->Show(true);
  }
  if (m->canSetTextureFromFile(TEXTURE_GAMEOBJECT3))
  {
    BLPSkinList3->Show(true);
    BLPSkinLabel3->Show(true);
    BLPSkinsLabel->Show(true);
  }

  bool ret = false;

  if (!skins.empty())
  {
    LOG_INFO << "Found" << skins.size() << "skins (total)";
    ret = FillSkinSelector(skins);

    if (!count) // No entries on .dbc and skins.txt
      count = (int)skins.size();

    if (ret)
    { // Don't call SetSkin without a skin
      int mySkin = randomSkins ? randint(0, (int)count-1) : 0;
      SetSkin(mySkin);
    }
  }

  return ret;
}

bool AnimControl::UpdateItemModel(WoWModel *m)
{
  int numPCRs = 0;

  std::set<GameFile *> alreadyUsedTextures;
  TextureSet skins;

  LOG_INFO << "Searching skins for" << m->itemName();

  wxString fn = m->itemName().toStdString().c_str();

  // change M2 to mdx
  fn = fn.BeforeLast(wxT('.')) + wxT(".mdx");

  // Check to see if it's a helmet model, if so cut the race-
  // and gender-specific part of the filename off
  if (fn.Find(wxT("\\head\\")) > wxNOT_FOUND || fn.Find(wxT("\\Head\\")) > wxNOT_FOUND)
    fn = fn.BeforeLast('_') + wxT(".mdx");

  // just get the file name, exclude the path.
  fn = fn.AfterLast(MPQ_SLASH);

  // query textures for model1
  QString query= QString("SELECT DISTINCT path, name, ParticleColorID, ItemDisplayInfo.ID FROM ItemDisplayInfo "
                         "LEFT JOIN TextureFileData ON TextureItemID1 = TextureFileData.TextureItemID "
                         "LEFT JOIN FileData ON TextureFileData.FileDataID = FileData.id "
                         "WHERE Model1 = \"%1\" COLLATE NOCASE").arg(fn.mb_str());
  sqlResult r = GAMEDATABASE.sqlQuery(query);

  if(r.valid && !r.empty())
  {
    for(size_t i = 0 ; i < r.values.size() ; i++)
    {
      TextureGroup grp;
      grp.base = TEXTURE_ITEM;
      grp.definedTexture = true;
      grp.count = 1;
      GameFile * tex = GAMEDIRECTORY.getFile(r.values[i][0] + r.values[i][1]);
      alreadyUsedTextures.insert(tex);
      grp.tex[0] = tex;
      int cdi = r.values[i][3].toInt();
      int pci = r.values[i][2].toInt(); // particleColorIndex, for replacing particle color
      if (pci)
      {
        grp.particleColInd = pci;
        QString pciquery = QString("SELECT StartColor1, MidColor1, EndColor1, "
	      "StartColor2, MidColor2, EndColor2, StartColor3, MidColor3, EndColor3 FROM ParticleColor "
	      "WHERE ID = %1;").arg(pci);
        sqlResult pcir = GAMEDATABASE.sqlQuery(pciquery);
        if(pcir.valid && !pcir.empty())
        {
          std::vector<Vec4D> cols;
          for (size_t j = 0; j < pcir.values[0].size(); j++)
          {
            cols.push_back(fromARGB(pcir.values[0][j].toInt()));
          }
          PCRList.push_back({ {cols[0],cols[1],cols[2]}, {cols[3],cols[4],cols[5]}, {cols[6],cols[7],cols[8]} });
          grp.PCRIndex = numPCRs;
          numPCRs++;
        }
      }
      else
          grp.PCRIndex = -1;

      if (grp.tex[0] != 0)
        skins.insert(grp);
      CDIToTexGp[cdi] = grp;
    }
  }

  // do the same for model2
  query= QString("SELECT DISTINCT path, name, ParticleColorID, ItemDisplayInfo.ID FROM ItemDisplayInfo  "
                 "LEFT JOIN TextureFileData ON TextureItemID2 = TextureFileData.TextureItemID "
                 "LEFT JOIN FileData ON TextureFileData.FileDataID = FileData.id "
                 "WHERE Model2 = \"%1\" COLLATE NOCASE").arg(fn.mb_str());
  r = GAMEDATABASE.sqlQuery(query);

  if(r.valid && !r.empty())
  {
    for(size_t i = 0 ; i < r.values.size() ; i++)
    {
      TextureGroup grp;
      grp.base = TEXTURE_ITEM;
      grp.definedTexture = true;
      grp.count = 1;
      GameFile * tex = GAMEDIRECTORY.getFile(r.values[i][0] + r.values[i][1]);
      alreadyUsedTextures.insert(tex);
      grp.tex[0] = tex;
      int cdi = r.values[i][3].toInt();
      int pci = r.values[i][2].toInt(); // particleColorIndex, for replacing particle color
      if (pci)
      {
        grp.particleColInd = pci;
        QString pciquery = QString("SELECT StartColor1, MidColor1, EndColor1, "
	      "StartColor2, MidColor2, EndColor2, StartColor3, MidColor3, EndColor3 FROM ParticleColor "
	      "WHERE ID = %1;").arg(pci);
        sqlResult pcir = GAMEDATABASE.sqlQuery(pciquery);
        if(pcir.valid && !pcir.empty())
        {
          std::vector<Vec4D> cols;
          for (size_t j = 0; j < pcir.values[0].size(); j++)
          {
            cols.push_back(fromARGB(pcir.values[0][j].toInt()));
          }
          PCRList.push_back({ {cols[0],cols[1],cols[2]}, {cols[3],cols[4],cols[5]}, {cols[6],cols[7],cols[8]} });
          grp.PCRIndex = numPCRs;
          numPCRs++;
        }
      }
      else
          grp.PCRIndex = -1;

      if (grp.tex[0] != 0)
        skins.insert(grp);
      CDIToTexGp[cdi] = grp;
    }
  }

  LOG_INFO << "Found" << skins.size() << "skins (Database)";

  // get all blp files that correspond to the model
  std::set<GameFile *> files;

  QString filterString = m->itemName().mid(m->itemName().lastIndexOf("\\")+1);
  filterString = "(?i)^.*" + filterString.left(filterString.lastIndexOf(".")) + ".*\\.blp";
  GAMEDIRECTORY.getFilteredFiles(files, filterString);

  if (files.size() != 0)
  {
    TextureGroup grp;
    grp.base = TEXTURE_ITEM;
    grp.definedTexture = false;
    grp.count = 1;
    for (std::set<GameFile *>::iterator it = files.begin(); it != files.end(); ++it)
    {
      GameFile * tex = *it;

      // use this alone texture only if not already used from database infos
      if(std::find(alreadyUsedTextures.begin(),alreadyUsedTextures.end(), tex) == alreadyUsedTextures.end())
      {
        grp.tex[0] = tex;
        skins.insert(grp);
      }
    }
  }

  // Search the model's directory for all BLPs to add to secondary "BLPskins" selector:
  std::vector<GameFile *> folderFiles;

  // get all files in the model's folder:
  GAMEDIRECTORY.getFilesForFolder(folderFiles, modelFolder, ".blp");

  if (folderFiles.begin() != folderFiles.end())
  {
    for (std::vector<GameFile *>::iterator it = folderFiles.begin(); it != folderFiles.end(); ++it)
    {
      TextureGroup grp;
      grp.base = TEXTURE_ITEM;
      grp.count = 1;
      GameFile * tex = *it;
      grp.tex[0] =  tex;
      grp.definedTexture = false;
      if (modelFolderChanged)
        BLPskins.insert(grp);
    }
  }

  // Creatures can have 1-3 textures that can be taken from game files.
  // But it varies from model to model which of the three textures can
  // be set this way.
  if (m->canSetTextureFromFile(TEXTURE_ITEM))
  {
    // if there's loads of skins in the folder then filling the selector becomes slow and
    // costly, so instead show a button to give users the option of filling the selector:
    if (modelFolderChanged && BLPskins.size() > 50)
    {
      BLPSkinsLabel->Show(true);
      BLPSkinLabel1->Show(true);
      showBLPList->Show(true);
    }
    else if (!BLPskins.empty())
    {
      BLPSkinsLabel->Show(true);
      if (modelFolderChanged || !BLPListFilled)
        FillBLPSkinSelector(BLPskins, true);
      BLPSkinList1->Show(true);
      BLPSkinLabel1->Show(true);
      BLPSkinsLabel->Show(true);
    }
  }

  bool ret = false;

  if (!skins.empty())
  {
    LOG_INFO << "Found" << skins.size() << "skins (Total)";
    ret = FillSkinSelector(skins);

    if (ret)
    {
      // Don't call SetSkin without a skin
      int mySkin = randomSkins ? randint(0, (int)skins.size()-1) : 0;
      SetSkin(mySkin);
    }
  }

  return ret;
}

void AnimControl::ActivateBLPSkinList()
{
  if (!BLPskins.empty())
  {
    showBLPList->Show(false);
    FillBLPSkinSelector(BLPskins, true);
    BLPSkinList1->Show(true);
    SyncBLPSkinList();
  }
}

void AnimControl::SyncBLPSkinList()
{
  BLPSkinList1->SetSelection(wxNOT_FOUND);
  BLPSkinList2->SetSelection(wxNOT_FOUND);
  BLPSkinList3->SetSelection(wxNOT_FOUND);

  // Configure BLPSkinLists (single skin selectors) to show same skins as the main texture selector, if possible
  std::vector<wxString> currTextures(3);

  int sel = skinList->GetSelection();
  if (sel < 0) // model not currently using a proper texture set, possibly custom
    return;
  TextureGroup *grp = (TextureGroup*) skinList->GetClientData(sel);


  for (size_t i = 0; i < 3; i++)
  {
    wxString texname;
    GameFile * tex = grp->tex[i];
    if (tex)
    {
      texname = tex->fullname().toStdString().c_str();
      texname = texname.AfterLast(MPQ_SLASH).BeforeLast('.');
    }
    currTextures[i] = texname;
  }

  // Set BLPSkinLists (single skin selector menus) to the same textures, if possible:
  if (BLPListFilled && BLPskins.size() > 0)
  {
    int num = 0;
    // fill our skin selector
    for (TextureSet::iterator it = BLPskins.begin(); it != BLPskins.end(); ++it)
    {
      GameFile * tex = it->tex[0];
      wxString texname = tex->fullname().toStdString().c_str();
      texname = texname.AfterLast(MPQ_SLASH).BeforeLast('.');
      if (texname == currTextures[0])
        BLPSkinList1->SetSelection(num);
      if (texname == currTextures[1])
        BLPSkinList2->SetSelection(num);
      if (texname == currTextures[2])
        BLPSkinList3->SetSelection(num);
      num++;
    }
  }
}

bool AnimControl::FillSkinSelector(TextureSet &skins)
{
  if (skins.size() < 1)
    return false;

  int num = 0;
  // fill our skin selector
  for (std::set<TextureGroup>::iterator it = skins.begin(); it != skins.end(); ++it)
  {
    GameFile * tex = it->tex[0];
    wxString texname = tex->fullname().toStdString().c_str();
    wxString selectorName = texname.AfterLast(MPQ_SLASH).BeforeLast('.');
    if (it->definedTexture)
      selectorName.MakeUpper();
    skinList->Append(selectorName);
    LOG_INFO << "TextureList[" << g_selModel->TextureList.size() << "] : added " 
             << texname.c_str() << " via FillSkinSelector";
    g_selModel->TextureList.push_back(tex);
    TextureGroup *grp = new TextureGroup(*it);
    skinList->SetClientData(num++, grp);
  }

  bool ret = (skins.size() > 0);
  //skins.clear();
  return ret;
}

bool AnimControl::FillBLPSkinSelector(TextureSet &skins, bool item)
{
  BLPListFilled = true;

  if (skins.size() > 0)
  {
    BLPSkinList1->Freeze();
    BLPSkinList2->Freeze();
    BLPSkinList3->Freeze();
    int num = 0;
    // fill our skin selector
    for (TextureSet::iterator it = skins.begin(); it != skins.end(); ++it)
    {
      GameFile * tex = it->tex[0];
      wxString texname = tex->fullname().toStdString().c_str();
      texname = texname.AfterLast(MPQ_SLASH).BeforeLast('.');
      if (!item)
        LOG_INFO << "TextureList[" << g_selModel->TextureList.size() << "] : added "
                 << texname.c_str() << " via FillBLPSkinSelector";
      g_selModel->TextureList.push_back(tex);
      TextureGroup *grp = new TextureGroup(*it);

      BLPSkinList1->Append(texname);
      BLPSkinList1->SetClientData(num, grp);

      if (!item)
      {
        BLPSkinList2->Append(texname);
        BLPSkinList3->Append(texname);
        BLPSkinList2->SetClientData(num, grp);
        BLPSkinList3->SetClientData(num, grp);
      }
      num++;
    }
    BLPSkinList1->Thaw();
    BLPSkinList2->Thaw();
    BLPSkinList3->Thaw();

    bool ret = (skins.size() > 0);
    //skins.clear();
    return ret;
  }
  else
    return false;
}

void AnimControl::OnButton(wxCommandEvent &event)
{
  if (!g_selModel)
    return;

  switch (event.GetId())
  {
    case ID_PLAY :
        g_selModel->currentAnim = g_selModel->animManager->GetAnim();
        g_selModel->animManager->Play();
        break;
    case ID_PAUSE :
        g_selModel->animManager->Pause();
        break;
    case ID_STOP :
        g_selModel->animManager->Stop();
        break;
    case ID_CLEARANIM :
        g_selModel->animManager->Clear();
        break;
    case ID_ADDANIM :
        g_selModel->animManager->AddAnim(selectedAnim, loopList->GetSelection());
        break;
    case ID_PREVANIM :
        g_selModel->animManager->PrevFrame();
        SetAnimFrame(g_selModel->animManager->GetFrame());
        break;
    case ID_NEXTANIM :
        g_selModel->animManager->NextFrame();
        SetAnimFrame(g_selModel->animManager->GetFrame());
        break;
    case ID_ANIM_SECONDARY_TEXT :
        {
          int count = wxAtoi(lockText->GetValue());
          if (count < 0)
            count = UPPER_BODY_BONES;
          if (count > BONE_MAX)
            count = BONE_MAX;
          g_selModel->animManager->SetSecondaryCount(count);
        }
        break;
    case ID_SHOW_BLP_SKINLIST:
        ActivateBLPSkinList();
        break;
  }
}

void AnimControl::OnCheck(wxCommandEvent &event)
{
	if (event.GetId() == ID_OLDSTYLE)
		bOldStyle = event.IsChecked();
	else if (event.GetId() == ID_ANIM_LOCK) {
		bLockAnims = event.IsChecked();

		if (bLockAnims == false) {
			animCList2->Enable(true);
			animCList2->Show(true);
			lockText->Enable(true);
			lockText->Show(true);
			animCList3->Enable(true);
			animCList3->Show(true);
			speedMouthSlider->Show(true);
			speedMouthLabel->Show(true);
			//btnPauseMouth->Show(true);
		} else {
			if (g_selModel)
				g_selModel->animManager->ClearSecondary();
			animCList2->Enable(false);
			animCList2->Show(false);
			lockText->Enable(false);
			lockText->Show(false);
			animCList3->Enable(false);
			animCList3->Show(false);
			speedMouthSlider->Show(false);
			speedMouthLabel->Show(false);
			//btnPauseMouth->Show(false);
		}
	} else if  (event.GetId() == ID_ANIM_NEXT) {
		bNextAnims = event.IsChecked();
		if (bNextAnims && g_selModel) {
			int NextAnimation = selectedAnim;
			for(size_t i=1; i<4; i++) {
				NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
				if (NextAnimation >= 0)
					g_selModel->animManager->AddAnim(NextAnimation, loopList->GetSelection());
				else
					break;
			}
		} else {
			g_selModel->animManager->SetCount(1);
		}
	}
}

void AnimControl::OnAnim(wxCommandEvent &event)
{
	if (event.GetId() == ID_ANIM) {
		if (g_selModel) {
			wxString val = animCList->GetValue();
			int first = val.Find('[')+1;
			int last = val.Find(']');
			selectedAnim = wxAtoi(val.Mid(first, last-first));
			
			if (bLockAnims) {
				//selectedAnim2 = -1;
				animCList2->SetSelection(event.GetSelection());
			}

			if (bOldStyle == true) {
				g_selModel->currentAnim = selectedAnim;
				g_selModel->animManager->Stop();
				g_selModel->animManager->SetAnim(0, selectedAnim, loopList->GetSelection());
				if (bNextAnims && g_selModel) {
					int NextAnimation = selectedAnim;
					for(size_t i=1; i<4; i++) {
						NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
						if (NextAnimation >= 0)
							g_selModel->animManager->AddAnim(NextAnimation, loopList->GetSelection());
						else
							break;
					}
				}
				g_selModel->animManager->Play();
				
				frameSlider->SetRange(g_selModel->anims[selectedAnim].timeStart, g_selModel->anims[selectedAnim].timeEnd);
				frameSlider->SetTickFreq(g_selModel->anims[selectedAnim].playSpeed, 1);
			}
		}

		//canvas->resetTime();
	} else if (event.GetId() == ID_ANIM_SECONDARY) {
		wxString val = animCList2->GetValue();
		int first = val.Find('[')+1;
		int last = val.Find(']');
		selectedAnim2 = wxAtoi(val.Mid(first, last-first));

		g_selModel->animManager->SetSecondary(selectedAnim2);
	} else if (event.GetId() == ID_ANIM_MOUTH) {
		wxString val = animCList3->GetValue();
		int first = val.Find('[')+1;
		int last = val.Find(']');
		selectedAnim3 = wxAtoi(val.Mid(first, last-first));

		//canvas->g_selModel->animManager->SetSecondary(selectedAnim2);
		g_selModel->animManager->SetMouth(event.GetSelection());
	}
}

void AnimControl::OnSkin(wxCommandEvent &event)
{
  if (g_selModel)
  {
    int sel = event.GetSelection();
    SetSkin(sel);
  }
}

void AnimControl::OnBLPSkin(wxCommandEvent &event)
{
  if (g_selModel)
  {
    int sel = event.GetSelection();
    int texnum;
    switch (event.GetId())
    {
      case ID_BLP_SKIN1:
        texnum = 1;
        break;
      case ID_BLP_SKIN2:
        texnum = 2;
        break;
      case ID_BLP_SKIN3:
        texnum = 3;
        break;
      default:
        return;
    }
    SetSingleSkin(sel, texnum);
  }
  skinList->SetSelection(wxNOT_FOUND);
}


void AnimControl::OnItemSet(wxCommandEvent &event)
{
	if (g_selWMO) {
		int sel = event.GetSelection();
		// -1 for no doodads
		g_selWMO->showDoodadSet(sel - 1);
	}
}

void AnimControl::OnSliderUpdate(wxCommandEvent &event)
{
	if (event.GetId() == ID_SPEED) {
		SetAnimSpeed(speedSlider->GetValue() / 10.0f);

	} else if (event.GetId() == ID_SPEED_MOUTH) {
		if (!g_selModel || !g_selModel->animManager)
			return;
		
		float speed = speedMouthSlider->GetValue() / 10.0f;
		g_selModel->animManager->SetMouthSpeed(speed);
		speedMouthLabel->SetLabel(wxString::Format(_("Speed: %.1fx"), speed));

	} else if (event.GetId() == ID_FRAME)
		SetAnimFrame(frameSlider->GetValue());

}

void AnimControl::OnLoop(wxCommandEvent &)
{
	if (bOldStyle == true) {
		g_selModel->animManager->Stop();
		g_selModel->animManager->SetAnim(0, selectedAnim, loopList->GetSelection());
		if (bNextAnims && g_selModel) {
			int NextAnimation = selectedAnim;
			for(size_t i=1; i<4; i++) {
				NextAnimation = g_selModel->anims[NextAnimation].NextAnimation;
				if (NextAnimation >= 0)
					g_selModel->animManager->AddAnim(NextAnimation, loopList->GetSelection());
				else
					break;
			}
		}
		g_selModel->animManager->Play();
	} 
}

void AnimControl::SetSkin(int num)
{
  std::vector<wxString> currTextures(3);

  if (num == -1)
    num = skinList->GetSelection();  // if we pass -1 to the func, we're redrawing the current skin
  if (num < 0) // model not currently using a proper texture set, possibly custom
  {
    g_selModel->replaceParticleColors = false;
    return;
  }
  TextureGroup *grp = (TextureGroup*) skinList->GetClientData(num);

  if (!grp) // invalid texture group
  {
    g_selModel->replaceParticleColors = false;
    return;
  }
  for (size_t i=0; i<grp->count; i++)
  {
    wxString texname;
    GameFile * tex = grp->tex[i];
    if (tex)
    {
      texname = tex->fullname().toStdString().c_str();
      texname = texname.AfterLast(MPQ_SLASH).BeforeLast('.');
    }
    currTextures[i] = texname;

    int base = grp->base + i;
    if (g_selModel->useReplaceTextures[base])
    {
      // refresh TextureList for further use
      for (ssize_t j=0; j<TEXTURE_MAX; j++)
      {
        if (base == g_selModel->specialTextures[j])
        {
          g_selModel->TextureList[j] = tex;
          break;
        }
      }
      g_selModel->replaceTextures[grp->base+i] = texturemanager.add(tex);
    }
  }

  if (grp->particleColInd && grp->PCRIndex > -1 && !g_modelViewer->modelControl->IsReplacingParticleColors())
  {
    g_selModel->replaceParticleColors = true;
    g_selModel->particleColorReplacements = PCRList[grp->PCRIndex];
  }
  else
    g_selModel->replaceParticleColors = false;

  skinList->Select(num);

  SyncBLPSkinList();
}

void AnimControl::SetSingleSkin(int num, int texnum)
{
  TextureGroup *grp;

  switch (texnum)
  {
    case 1:
      grp = (TextureGroup*) BLPSkinList1->GetClientData(num);
      break;
    case 2:
      grp = (TextureGroup*) BLPSkinList2->GetClientData(num);
      break;
    case 3:
      grp = (TextureGroup*) BLPSkinList3->GetClientData(num);
      break;
    default:
      return;
  }

  int base = grp->base + texnum - 1;
  if (g_selModel->useReplaceTextures[base])
  {
    GameFile * tex = grp->tex[0];
    LOG_INFO << "SETSINGLESKIN skin = " << tex->fullname();
    // refresh TextureList for further use
    for (ssize_t j=0; j<TEXTURE_MAX; j++)
    {
      if (base == g_selModel->specialTextures[j])
      {
        g_selModel->TextureList[j] = tex;
        break;
      }
    }
    g_selModel->replaceTextures[base] = texturemanager.add(tex);
  }
}

int AnimControl::AddSkin(TextureGroup grp)
{
  skinList->Append(wxT("Custom"));
  int count = skinList->GetCount() - 1;
  TextureGroup *group = new TextureGroup(grp);
  skinList->SetClientData(count, group);
  SetSkin(count);
  return count;
}

void AnimControl::SetAnimSpeed(float speed)
{
	if (!g_selModel || !g_selModel->animManager)
		return;

	g_selModel->animManager->SetSpeed(speed);
	
	speedLabel->SetLabel(wxString::Format(_("Speed: %.1fx"), speed));
}

void AnimControl::SetAnimFrame(size_t frame)
{
	if (!g_selModel || !g_selModel->animManager)
		return;

	g_selModel->animManager->SetFrame(frame);
	
	size_t frameNum = (frame - g_selModel->anims[g_selModel->currentAnim].timeStart);

	frameLabel->SetLabel(wxString::Format(_("Frame: %i"), frameNum));
	frameSlider->SetValue((int)frame);
}
