#include "../Platform/stdafx.h"
#include "Options.h"
#include "../Input/KeyMapping.h"
#include "../Rendering/LevelRenderer.h"
#include "../Textures/Textures.h"
#include "..\Minecraft.World\net.minecraft.locale.h"
#include "..\Minecraft.World\Language.h"
#include "..\Minecraft.World\File.h"
#include "..\Minecraft.World\BufferedReader.h"
#include "..\Minecraft.World\DataInputStream.h"
#include "..\Minecraft.World\InputStreamReader.h"
#include "..\Minecraft.World\FileInputStream.h"
#include "..\Minecraft.World\FileOutputStream.h"
#include "..\Minecraft.World\DataOutputStream.h"
#include "..\Minecraft.World\StringHelpers.h"

// 4J - the Option sub-class used to be an java enumerated type, trying to
// emulate that functionality here
const Options::Option Options::Option::options[17] = {
    Options::Option(L"options.music", true, false),
    Options::Option(L"options.sound", true, false),
    Options::Option(L"options.invertMouse", false, true),
    Options::Option(L"options.sensitivity", true, false),
    Options::Option(L"options.renderDistance", false, false),
    Options::Option(L"options.viewBobbing", false, true),
    Options::Option(L"options.anaglyph", false, true),
    Options::Option(L"options.advancedOpengl", false, true),
    Options::Option(L"options.framerateLimit", false, false),
    Options::Option(L"options.difficulty", false, false),
    Options::Option(L"options.graphics", false, false),
    Options::Option(L"options.ao", false, true),
    Options::Option(L"options.guiScale", false, false),
    Options::Option(L"options.fov", true, false),
    Options::Option(L"options.gamma", true, false),
    Options::Option(L"options.renderClouds", false, true),
    Options::Option(L"options.particles", false, false),
};

const Options::Option* Options::Option::MUSIC = &Options::Option::options[0];
const Options::Option* Options::Option::SOUND = &Options::Option::options[1];
const Options::Option* Options::Option::INVERT_MOUSE =
    &Options::Option::options[2];
const Options::Option* Options::Option::SENSITIVITY =
    &Options::Option::options[3];
const Options::Option* Options::Option::RENDER_DISTANCE =
    &Options::Option::options[4];
const Options::Option* Options::Option::VIEW_BOBBING =
    &Options::Option::options[5];
const Options::Option* Options::Option::ANAGLYPH = &Options::Option::options[6];
const Options::Option* Options::Option::ADVANCED_OPENGL =
    &Options::Option::options[7];
const Options::Option* Options::Option::FRAMERATE_LIMIT =
    &Options::Option::options[8];
const Options::Option* Options::Option::DIFFICULTY =
    &Options::Option::options[9];
const Options::Option* Options::Option::GRAPHICS =
    &Options::Option::options[10];
const Options::Option* Options::Option::AMBIENT_OCCLUSION =
    &Options::Option::options[11];
const Options::Option* Options::Option::GUI_SCALE =
    &Options::Option::options[12];
const Options::Option* Options::Option::FOV = &Options::Option::options[13];
const Options::Option* Options::Option::GAMMA = &Options::Option::options[14];
const Options::Option* Options::Option::RENDER_CLOUDS =
    &Options::Option::options[15];
const Options::Option* Options::Option::PARTICLES =
    &Options::Option::options[16];

const Options::Option* Options::Option::getItem(int id) { return &options[id]; }

Options::Option::Option(const std::wstring& captionId, bool hasProgress,
                        bool isBoolean)
    : _isProgress(hasProgress), _isBoolean(isBoolean), captionId(captionId) {}

bool Options::Option::isProgress() const { return _isProgress; }

bool Options::Option::isBoolean() const { return _isBoolean; }

int Options::Option::getId() const { return (int)(this - options); }

std::wstring Options::Option::getCaptionId() const { return captionId; }

const std::wstring Options::RENDER_DISTANCE_NAMES[] = {
    L"options.renderDistance.far", L"options.renderDistance.normal",
    L"options.renderDistance.short", L"options.renderDistance.tiny"};
const std::wstring Options::DIFFICULTY_NAMES[] = {
    L"options.difficulty.peaceful", L"options.difficulty.easy",
    L"options.difficulty.normal", L"options.difficulty.hard"};
const std::wstring Options::GUI_SCALE[] = {
    L"options.guiScale.auto", L"options.guiScale.small",
    L"options.guiScale.normal", L"options.guiScale.large"};
const std::wstring Options::FRAMERATE_LIMITS[] = {
    L"performance.max", L"performance.balanced", L"performance.powersaver"};

const std::wstring Options::PARTICLES[] = {L"options.particles.all",
                                           L"options.particles.decreased",
                                           L"options.particles.minimal"};

// 4J added
void Options::init() {
    music = 1;
    sound = 1;
    sensitivity = 0.5f;
    invertYMouse = false;
    viewDistance = 0;
    bobView = true;
    anaglyph3d = false;
    advancedOpengl = false;
    framerateLimit = 2;
    fancyGraphics = true;
    ambientOcclusion = true;
    renderClouds = true;
    skin = L"Default";

    keyUp = new KeyMapping(L"key.forward", Keyboard::KEY_W);
    keyLeft = new KeyMapping(L"key.left", Keyboard::KEY_A);
    keyDown = new KeyMapping(L"key.back", Keyboard::KEY_S);
    keyRight = new KeyMapping(L"key.right", Keyboard::KEY_D);
    keyJump = new KeyMapping(L"key.jump", Keyboard::KEY_SPACE);
    keyBuild = new KeyMapping(L"key.inventory", Keyboard::KEY_E);
    keyDrop = new KeyMapping(L"key.drop", Keyboard::KEY_Q);
    keyChat = new KeyMapping(L"key.chat", Keyboard::KEY_T);
    keySneak = new KeyMapping(L"key.sneak", Keyboard::KEY_LSHIFT);
    keyAttack = new KeyMapping(L"key.attack", -100 + 0);
    keyUse = new KeyMapping(L"key.use", -100 + 1);
    keyPlayerList = new KeyMapping(L"key.playerlist", Keyboard::KEY_TAB);
    keyPickItem = new KeyMapping(L"key.pickItem", -100 + 2);
    keyToggleFog = new KeyMapping(L"key.fog", Keyboard::KEY_F);

    keyMappings[0] = keyAttack;
    keyMappings[1] = keyUse;
    keyMappings[2] = keyUp;
    keyMappings[3] = keyLeft;
    keyMappings[4] = keyDown;
    keyMappings[5] = keyRight;
    keyMappings[6] = keyJump;
    keyMappings[7] = keySneak;
    keyMappings[8] = keyDrop;
    keyMappings[9] = keyBuild;
    keyMappings[10] = keyChat;
    keyMappings[11] = keyPlayerList;
    keyMappings[12] = keyPickItem;
    keyMappings[13] = keyToggleFog;

    minecraft = NULL;
    // optionsFile = NULL;

    difficulty = 2;
    hideGui = false;
    thirdPersonView = false;
    renderDebug = false;
    lastMpIp = L"";

    isFlying = false;
    smoothCamera = false;
    fixedCamera = false;
    flySpeed = 1;
    cameraSpeed = 1;
    guiScale = 0;
    particles = 0;
    fov = 0;
    gamma = 0;
}

Options::Options(Minecraft* minecraft, File workingDirectory) {
    init();
    this->minecraft = minecraft;
    optionsFile = File(workingDirectory, L"options.txt");
}

Options::Options() { init(); }

std::wstring Options::getKeyDescription(int i) {
    Language* language = Language::getInstance();
    return language->getElement(keyMappings[i]->name);
}

std::wstring Options::getKeyMessage(int i) {
    int key = keyMappings[i]->key;
    if (key < 0) {
        return I18n::get(L"key.mouseButton", key + 101);
    } else {
        return Keyboard::getKeyName(keyMappings[i]->key);
    }
}

void Options::setKey(int i, int key) {
    keyMappings[i]->key = key;
    save();
}

void Options::set(const Options::Option* item, float fVal) {
    if (item == Option::MUSIC) {
        music = fVal;
#ifdef _XBOX
        minecraft->soundEngine->updateMusicVolume(fVal * 2.0f);
#else
        minecraft->soundEngine->updateMusicVolume(fVal);
#endif
    }
    if (item == Option::SOUND) {
        sound = fVal;
#ifdef _XBOX
        minecraft->soundEngine->updateSoundEffectVolume(fVal * 2.0f);
#else
        minecraft->soundEngine->updateSoundEffectVolume(fVal);
#endif
    }
    if (item == Option::SENSITIVITY) {
        sensitivity = fVal;
    }
    if (item == Option::FOV) {
        fov = fVal;
    }
    if (item == Option::GAMMA) {
        gamma = fVal;
    }
}

void Options::toggle(const Options::Option* option, int dir) {
    if (option == Option::INVERT_MOUSE) invertYMouse = !invertYMouse;
    if (option == Option::RENDER_DISTANCE)
        viewDistance = (viewDistance + dir) & 3;
    if (option == Option::GUI_SCALE) guiScale = (guiScale + dir) & 3;
    if (option == Option::PARTICLES) particles = (particles + dir) % 3;

    // 4J-PB - changing
    // if (option == Option::VIEW_BOBBING) bobView = !bobView;
    if (option == Option::VIEW_BOBBING)
        ((dir == 0) ? bobView = false : bobView = true);
    if (option == Option::RENDER_CLOUDS) renderClouds = !renderClouds;
    if (option == Option::ADVANCED_OPENGL) {
        advancedOpengl = !advancedOpengl;
        minecraft->levelRenderer->allChanged();
    }
    if (option == Option::ANAGLYPH) {
        anaglyph3d = !anaglyph3d;
        minecraft->textures->reloadAll();
    }
    if (option == Option::FRAMERATE_LIMIT)
        framerateLimit = (framerateLimit + dir + 3) % 3;

    // 4J-PB - Change for Xbox
    // if (option ==  Option::DIFFICULTY) difficulty = (difficulty + dir) & 3;
    if (option == Option::DIFFICULTY) difficulty = (dir) & 3;

    app.DebugPrintf("Option::DIFFICULTY = %d", difficulty);

    if (option == Option::GRAPHICS) {
        fancyGraphics = !fancyGraphics;
        minecraft->levelRenderer->allChanged();
    }
    if (option == Option::AMBIENT_OCCLUSION) {
        ambientOcclusion = !ambientOcclusion;
        minecraft->levelRenderer->allChanged();
    }

    // 4J-PB - don't do the file save on the xbox
    // save();
}

float Options::getProgressValue(const Options::Option* item) {
    if (item == Option::FOV) return fov;
    if (item == Option::GAMMA) return gamma;
    if (item == Option::MUSIC) return music;
    if (item == Option::SOUND) return sound;
    if (item == Option::SENSITIVITY) return sensitivity;
    return 0;
}

bool Options::getBooleanValue(const Options::Option* item) {
    // 4J - was a switch statement which we can't do with our Option:: pointer
    // types
    if (item == Option::INVERT_MOUSE) return invertYMouse;
    if (item == Option::VIEW_BOBBING) return bobView;
    if (item == Option::ANAGLYPH) return anaglyph3d;
    if (item == Option::ADVANCED_OPENGL) return advancedOpengl;
    if (item == Option::AMBIENT_OCCLUSION) return ambientOcclusion;
    if (item == Option::RENDER_CLOUDS) return renderClouds;
    return false;
}

std::wstring Options::getMessage(const Options::Option* item) {
    // 4J TODO, should these wstrings append rather than add?

    Language* language = Language::getInstance();
    std::wstring caption = language->getElement(item->getCaptionId()": "L    ;

    if (item->isProgress())
	{
        float progressValue = getProgressValue(item);

        if (item == Option::SENSITIVITY) {
            if (progressValue == 0) {
                return caption + language->getElem"options.sensitivity.min"     );
            }
            if (progressValue == 1) {
                return caption + language->getElem"options.sensitivity.max"     );
            }
                        return caption + _toString<int>((int) (progressValue * 200)"%" L   ;
        } else if (item == Option::FOV) {
            if (progressValue == 0) {
                                return caption + language->getElem"options.fov.min"     );
            }
            if (progressValue == 1) {
                                return caption + language->getElem"options.fov.max"     );
            }
            return caption + _toString<int>((int)(70 + progressValue * 40));
        } else if (item == Option::GAMMA) {
            if (progressValue == 0) {
                                return caption + language->getElem"options.gamma.min"     );
            }
            if (progressValue == 1) {
                                return caption + language->getElem"options.gamma.max"     );
            }
                        return captio"+" L    + _toString<int>((int) (progressValue * 100)"%" L   ;
        } else {
            if (progressValue == 0) {
                return caption + language->getElem"options.off"     );
            }
            return caption + _toString<int>((int) (progressValue * 100)"%" L   ;
        }
    } else if (item->isBoolean())
	{
        bool booleanValue = getBooleanValue(item);
        if (booleanValue) {
            return caption + language->getElem"options.on"     );
        }
        return caption + language->getElem"options.off"     );
    }
	else if (item == Option::RENDER_DISTANCE)
	{
        return caption +
               language->getElement(RENDER_DISTANCE_NAMES[viewDistance]);
    }
	else if (item == Option::DIFFICULTY)
	{
        return caption + language->getElement(DIFFICULTY_NAMES[difficulty]);
    }
	else if (item == Option::GUI_SCALE)
	{
        return caption + language->getElement(GUI_SCALE[guiScale]);
	}
	else if (item == Option::PARTICLES)
	{
        return caption + language->getElement(PARTICLES[particles]);
    }
	else if (item == Option::FRAMERATE_LIMIT)
	{
        return caption + I18n::get(FRAMERATE_LIMITS[framerateLimit]);
    }
	else if (item == Option::GRAPHICS)
	{
        if (fancyGraphics) {
            return caption + language->getElem"options.graphics.fancy"     );
        }
        return caption + language->getElem"options.graphics.fast"     );
    }

    return caption;
}

void Options::load(  // 4J - removed try/catch //    try {     
    if (!optionsFile.exists())
        retur  // 4J - was new BufferedReader(new FileReader(optionsFile));     
    BufferedReader* br = new BufferedReader(
        new InputStreamReader(new FileInputStream(optionsFile)));

    std::wstrin "" line = L  ;
    while ((
               line = br->readLi
                      ""()  // 4J - was check against NULL - do we need to
                            // distinguish between empty lines and a fail
                            // here?        // 4J - removed try/catch // try
                            // {          
                      std::wstring cmds[2];
               int splitpos = (":")line.find(L   );
               if (splitpos == wstring::npos) {
                   cmds[0] = line;
                   "" cmds[1] = L  ;
               } else {
                   cmds[0] = line.substr(0, splitpos);
                   cmds[1] = line.substr(splitpos, line.length() - splitpos);
               }

               i "music"[0] == L       ) music = readFloat(cmds[1]);
           i "sound"[0] == L       ) sound = readFloat(cmds[1]);
    i "mouseSensitivity"               ) sensitivity = readFloat(cmds[1]);
                                i"fov"ds[0] == L     ) fov = readFloat(cmds[1]);
                                i"gamma"[0] == L       ) gamma = readFloat(cmds[1]);
                i"invertYMouse"L              ) invertYMou"true"mds[1]==L      ;
                i"viewDistance"L              ) viewDistance = _fromString<int>(cmds[1]);
                i"guiScale" == L          ) guiScale =_fromString<int>(cmds[1]);
                                i"particles"== L           ) particles = _fromString<int>(cmds[1]);
                i"bobView"] == L         ) bobVi"true"mds[1]==L      ;
                i"anaglyph3d"= L            ) anaglyph"true"mds[1]==L      ;
                i"advancedOpengl"               ) advancedOpen"true"mds[1]==L      ;
                i"fpsLimit" == L          ) framerateLimit = _fromString<int>(cmds[1]);
                i"difficulty"= L            ) difficulty = _fromString<int>(cmds[1]);
                i"fancyGraphics"               ) fancyGraphi"true"mds[1]==L      ;
                i"ao"mds[0] == L    ) ambientOcclusi"true"mds[1]==L      ;
                                i"clouds"0] == L        ) renderClou"true"mds[1]==L      ;
                i"skin"s[0] == L      ) skin = cmds[1];
                i"lastServer"= L            ) lastMpIp = cmds[1];

                for (int i = 0; i < keyMappings_length; i++) {
                    if"key_"[0] == (L       + keyMappings[i]->name))
					{
                            keyMappings[i]->key = _fromString<int>(cmds[1]);
                        }
                    //            } catch (Exception e) { //
                    //            System.out.println("Skipping bad option: " +
                    //            line); //
                    //            }             //KeyMapping.resetMapping(); //
                    //            4J Not implemented               
                    //    } catch (Exception e) { // System.out.println("Failed
                    //    to load options"); //        e.printStackTrace(); //
                    //    }       
                           


                }

                float Options::readFloat(std::wstring std::string) {
                    "true" f(std::string == L      ) retu
                        "false" if (std::string == L       ) return 0;
    return _fromString<float>(std::s// 4J - try/catch removeds//    try {   // 4J - original used a PrintWriter & FileWriter, but seems a bit much implementing these just to do this                                   
		FileOutputStream fos = FileOutputStream(optionsFile);
		DataOutputSt//        PrintWriter pw = new PrintWriter(new FileWriter(optionsFile));                    "music:"       

		dos.writeChars(L   "\n"  + _toString<float>(music)"sound:");
        dos.writeChars(L   "\n"  + _toString<float>(sound)"invertYMouse:"    dos.writeChars(L       "true"   +"false"strin"\n"vertYMouse ? L       : L   "mouseSensitivity:"     dos.writeChars(L                    + _toString<"fov:"(sensitivity));
		dos.writeChars(L       + _t"gamma:"float>(fov));
		dos.writeChars(L         + _toString<"viewDistance:"
        dos.writeChars(L                + _toString<int>("guiScale:"e));
        dos.writeChars(L            + _toSt"particles:"iScale));
		dos.writeChars(L             + _toString<in"bobView:"es));
        dos.writ"true"(L  "false"  + std::wstring(bobView ? L"anaglyph3d:"    ));
        dos.write"true"L   "false"    + std::wstring(anaglyph3"advancedOpengl:"     ));
        dos.writeCha"true"    "false"     + std::wstring(advanced"fpsLimit:"      : L       ));
        dos.writeChars(L            + _t"difficulty:"framerateLimit));
        dos.writeChars(L              "fancyGraphics:"(difficulty));
        dos.w "true" ars("false"           +
                         std::wstring(fa "ao:" aphics ? L       : L       ));
"true"  do"false"Chars(L      + std::ws"clouds:"ientOcclusion ? L       : L       ));
dos.writeChars(L   "skin:" + _toString<bool>(renderClouds));
"lastServer:" riteChars(L        + skin);
dos.writeChars(L              + lastMpIp);

for (int i = "key_" < keyMappings_length; i++)
":"
            dos.writeChars(L       + keyMappings[i]->name + L    + _toSt//    } catch (Exception e) {)//        System.out.println("Failed to save options"); //        e.printStackTrace(); //    }                          
                              
       


                }

                bool Options::isCloudsOn() {
                    return viewDistance < 2 && renderClouds;
                }