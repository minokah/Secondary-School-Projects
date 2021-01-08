// untitled rpg game by brandon 2019

// bugfix: controller works fine now so its good
// might abandon controller?? maybe

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;
using namespace sf;

// very shitty global variables, but im too lazy to modify my functions to account for these
View mainView(Vector2f(486, 348), Vector2f(1024, 768));
int moveIncrement = 3;
Font sansRegular, sansLight, sansBold;

// [ STRUCTURES ]
struct NPC { // World NPC
    NPC(string type, Vector2f pos, string name, string file) {
        tx_npc.loadFromFile(file);
        spr_npc.setTexture(tx_npc);
        spr_npc.setScale(2, 2);
        spr_npc.setPosition(pos);
        npc_type = type;
        npc_name = name;

        if (type == "healer1" || type == "healer2" || type == "healer3") text_dialoguetrigger.setString("[ F ] Heal");
        else if (type == "shopkeeper1" || type == "shopkeeper2" || type == "shopkeeper3") text_dialoguetrigger.setString("[ F ] Shop");
        else text_dialoguetrigger.setString("[ F ] Talk");
        text_dialoguetrigger.setFont(sansBold);
        text_dialoguetrigger.setCharacterSize(15);
        text_dialoguetrigger.setPosition(Vector2f(pos.x - 10, pos.y - 25));
        text_dialoguetrigger.setFillColor(Color(255, 0, 220, 0));
        text_dialoguetrigger.setOutlineColor(Color(0, 0, 0));
        text_dialoguetrigger.setOutlineThickness(0);
    }

    string npc_type, npc_name;
    Texture tx_npc;
    Sprite spr_npc;
    Text text_dialoguetrigger;
};

struct InventoryItem { // inventory objects (cleans up a lot of lines)
    RectangleShape Container, Icon;
    Texture image;
    Text Label, Amount;
    InventoryItem(string name1, string type, string imageFile, int x, int yOffset) {
        itemName = name1;
        itemType = type;
        image.loadFromFile(imageFile);
        Icon.setTexture(&image);

        Container.setSize(Vector2f(270, 30));
        Container.setPosition(x, 25 + yOffset);
        Container.setFillColor(Color(30, 30, 30));
        Icon.setSize(Vector2f(30, 30));
        Icon.setPosition(Container.getPosition());
        Label.setString(name1);
        Label.setFont(sansRegular);
        Label.setCharacterSize(15);
        Label.setPosition(Vector2f(Container.getPosition().x + 55, Container.getPosition().y + 6));
        Amount.setString("");
        Amount.setFont(sansRegular);
        Amount.setCharacterSize(17);
        if (type == "consumable") Amount.setPosition(Vector2f(Container.getPosition().x + 235, Container.getPosition().y + 5));
        else if (type == "weapon") Amount.setPosition(Vector2f(Container.getPosition().x + 185, Container.getPosition().y + 5));
    }

    string itemName, itemType;
    int itemAmount = 0;
};

// [ Functions ]
bool playerColliding = false;
bool npcCollision(Sprite &player, Sprite npc) { // Checks for collision with NPCs; moves accordingly.
    if (player.getGlobalBounds().intersects(npc.getGlobalBounds()) && !playerColliding) {
        if (player.getPosition().x < npc.getPosition().x) {
            player.move(-moveIncrement, 0);
            mainView.move(-moveIncrement, 0);
        }
        if (player.getPosition().x > npc.getPosition().x) {
            player.move(moveIncrement, 0);
            mainView.move(moveIncrement, 0);
        }
        if (player.getPosition().y < npc.getPosition().y) {
            player.move(0, -moveIncrement);
            mainView.move(0, -moveIncrement);
        }
        if (player.getPosition().y > npc.getPosition().y) {
            player.move(0, moveIncrement);
            mainView.move(0, moveIncrement);
        }
        playerColliding = true;
        return true;
    }
    else playerColliding = false;
    return false;
}

void barrierCollision(Sprite &player, RectangleShape barrier) { // Barrier collisions
    if (player.getGlobalBounds().intersects(barrier.getGlobalBounds()) && !playerColliding) {
        if (player.getPosition().x < barrier.getPosition().x) {
            player.move(-moveIncrement, 0);
            mainView.move(-moveIncrement, 0);
        }
        if (player.getPosition().x > barrier.getPosition().x) {
            player.move(moveIncrement, 0);
            mainView.move(moveIncrement, 0);
        }
        if (player.getPosition().y < barrier.getPosition().y) {
            player.move(0, -moveIncrement);
            mainView.move(0, -moveIncrement);
        }
        if (player.getPosition().y > barrier.getPosition().y) {
            player.move(0, moveIncrement);
            mainView.move(0, moveIncrement);
        }
        playerColliding = true;
    }
    else playerColliding = false;
}

void progressLoading(RenderWindow &window, Text &text, string msg, RectangleShape &wheel) {  // why not
    text.setString(msg);
    wheel.rotate(40);
    window.clear();
    window.draw(text);
    window.draw(wheel);
    window.display();
}

int main()
{
    srand(time(0));
    RenderWindow app(VideoMode(1024, 768), "Untitled RPG Game | Brandon Luu 2019");
    app.setFramerateLimit(60);

    // [ Fonts ]
    sansRegular.loadFromFile("Assets\\Fonts\\SansRegular.otf");
    sansLight.loadFromFile("Assets\\Fonts\\SansLight.otf");
    sansBold.loadFromFile("Assets\\Fonts\\SansBold.otf");

    // Load screen
    Texture texLoadWheel;
    texLoadWheel.loadFromFile("Assets\\Other\\load.png");
    RectangleShape loadWheel(Vector2f(60, 60));
    loadWheel.setPosition(Vector2f(app.getSize().x - 60, app.getSize().y - 55));
    loadWheel.setTexture(&texLoadWheel);
    loadWheel.setOrigin(30, 30);
    Text text_loadingText("[ ... ]", sansRegular, 25);
    text_loadingText.setPosition(Vector2f(25, app.getSize().y - 50));
    progressLoading(app, text_loadingText, "[ Loading Containers ]", loadWheel);

    vector<InventoryItem> uiInventory;
    vector<InventoryItem> combatInventory;
    vector<InventoryItem> shopInventory;
    vector<RectangleShape> mapBarriers;
    map<string, int> PlayerFile;
    map<string, string> map_dialogue;
    vector<NPC> spr_npcs;

    // [ Player Variables ]
    progressLoading(app, text_loadingText, "[ Loading Game Variables ]", loadWheel);
    int playerSpriteOffset = 0;
    string currentNPC, inputMethod = "kb";
    float ctrl_x, ctrl_y; // Controller Left/Right stick values for movement

    PlayerFile["player_weapon_equipped"] = 5; // Defaults player variables
    PlayerFile["inv_health"] = 100;
    PlayerFile["inv_gold"] = 0;
    PlayerFile["player_x"] = 4093;
    PlayerFile["player_y"] = 8055;
    PlayerFile["view_x"] = 4119;
    PlayerFile["view_y"] = 7569;
    PlayerFile["inv_apple_amt"] = 1;
    PlayerFile["inv_spcapple_amt"] = 1;
    PlayerFile["inv_meat_amt"] = 1;
    PlayerFile["inv_herb_amt"] = 1;
    PlayerFile["inv_hpotion_amt"] = 1;

    Clock footstepTick;
    int footstepCounter = 0;
    Vector2f posBeforeEncounter, viewBeforeEncounter, viewBeforeCutscene;

    // [ Game Flags ]
    Clock frameTick, uiTick;
    bool isPlaying = false, isStartingGame = false, isSelectingCharacter = false, isShowingBinds = false, isSaving = false, saveExists = false; // Menu
    int uiMenuSelection = 1, uiCharacterSelected = 1, uiMenuStyle = 1;
    bool isInDialogue = false, isPaused = false, isinInventory = false, isinCombat = false, isFleeingCombat = false, isCombatInventory = false, isCombatSelection = false;
    bool isinShop = false, isCombatPlayerAttacking = false, isCombatEnemyAttacking = false, globalSoundPlayed = false, musicAlreadyPlaying = false, enemyDefeated = false;
    bool playerDefeated = false, playerHealed = false, worldMusicPlaying = false, firstIntroBlur = false, introCutscenePlayed = false;
    int currentDialogue = 0, uiInventorySelection = 1, combatEnemyNextMove;

    // [ Game Music/SFX ]
    progressLoading(app, text_loadingText, "[ Loading General Sounds/SFX ]", loadWheel);
    Music menuMusic, menuMusic2, fightMusic, world1Music;
    fightMusic.openFromFile("Assets\\Music\\fight.ogg");
    world1Music.openFromFile("Assets\\Music\\world1.ogg");

    menuMusic.setLoop(true);
    menuMusic2.setLoop(true);
    fightMusic.setLoop(true);
    world1Music.setLoop(true);

    Sound pick, select, save, dialogue, alert, inventory;
    SoundBuffer pickBuffer, selectBuffer, saveBuffer, dialogueBuffer, alertBuffer, inventoryBuffer;
    pickBuffer.loadFromFile("Assets\\Music\\pick.ogg");
    selectBuffer.loadFromFile("Assets\\Music\\select.ogg");
    saveBuffer.loadFromFile("Assets\\Music\\save.ogg");
    dialogueBuffer.loadFromFile("Assets\\Music\\dialogue.ogg");
    alertBuffer.loadFromFile("Assets\\Music\\alert.ogg");
    inventoryBuffer.loadFromFile("Assets\\Music\\inventory.ogg");
    pick.setBuffer(pickBuffer);
    select.setBuffer(selectBuffer);
    save.setBuffer(saveBuffer);
    dialogue.setBuffer(dialogueBuffer);
    alert.setBuffer(alertBuffer);
    inventory.setBuffer(inventoryBuffer);

    SoundBuffer sword1Buffer, sword2Buffer, sword3Buffer, eyelander1Buffer, eyelander2Buffer, eyelander3Buffer, healBuffer; // Combat weapon sounds
    SoundBuffer spear1Buffer, spear2Buffer, spear3Buffer, staff1Buffer, staff2Buffer, staff3Buffer, revolver1Buffer, revolver2Buffer, revolver3Buffer;
    SoundBuffer crossbow1Buffer, crossbow2Buffer, crossbow3Buffer;
    sword1Buffer.loadFromFile("Assets\\Music\\sword1.ogg");
    sword2Buffer.loadFromFile("Assets\\Music\\sword2.ogg");
    sword3Buffer.loadFromFile("Assets\\Music\\sword3.ogg");
    eyelander1Buffer.loadFromFile("Assets\\Music\\eyelander1.ogg");
    eyelander2Buffer.loadFromFile("Assets\\Music\\eyelander2.ogg");
    eyelander3Buffer.loadFromFile("Assets\\Music\\eyelander3.ogg");
    spear1Buffer.loadFromFile("Assets\\Music\\spear1.ogg");
    spear2Buffer.loadFromFile("Assets\\Music\\spear2.ogg");
    spear3Buffer.loadFromFile("Assets\\Music\\spear3.ogg");
    staff1Buffer.loadFromFile("Assets\\Music\\staff1.ogg");
    staff2Buffer.loadFromFile("Assets\\Music\\staff2.ogg");
    staff3Buffer.loadFromFile("Assets\\Music\\staff3.ogg");
    revolver1Buffer.loadFromFile("Assets\\Music\\revolver1.ogg");
    revolver2Buffer.loadFromFile("Assets\\Music\\revolver2.ogg");
    revolver3Buffer.loadFromFile("Assets\\Music\\revolver3.ogg");
    crossbow1Buffer.loadFromFile("Assets\\Music\\crossbow1.ogg");
    crossbow2Buffer.loadFromFile("Assets\\Music\\crossbow2.ogg");
    crossbow3Buffer.loadFromFile("Assets\\Music\\crossbow3.ogg");
    healBuffer.loadFromFile("Assets\\Music\\heal.ogg");
    Sound sword1, sword2, sword3, eyelander1, eyelander2, eyelander3, spear1, spear2, spear3, staff1, staff2, staff3, revolver1, revolver2, revolver3, heal;
    Sound crossbow1, crossbow2, crossbow3;
    sword1.setBuffer(sword1Buffer);
    sword2.setBuffer(sword2Buffer);
    sword3.setBuffer(sword3Buffer);
    eyelander1.setBuffer(eyelander1Buffer);
    eyelander2.setBuffer(eyelander2Buffer);
    eyelander3.setBuffer(eyelander3Buffer);
    spear1.setBuffer(spear1Buffer);
    spear2.setBuffer(spear2Buffer);
    spear3.setBuffer(spear3Buffer);
    staff1.setBuffer(staff1Buffer);
    staff2.setBuffer(staff2Buffer);
    staff3.setBuffer(staff3Buffer);
    revolver1.setBuffer(revolver1Buffer);
    revolver2.setBuffer(revolver2Buffer);
    revolver3.setBuffer(revolver3Buffer);
    crossbow1.setBuffer(crossbow1Buffer);
    crossbow2.setBuffer(crossbow2Buffer);
    crossbow3.setBuffer(crossbow3Buffer);
    heal.setBuffer(healBuffer);

    // [ Menu Interface ]
    progressLoading(app, text_loadingText, "[ Loading UI ]", loadWheel);
    RectangleShape uiBlur(Vector2f(app.getSize().x, app.getSize().y));
    uiBlur.setFillColor(Color(0, 0, 0, 255));

    RectangleShape menuAnim(Vector2f(1024, 633));
    RectangleShape menuKeybindsKB(Vector2f(1024, 633));
    RectangleShape menuKeybindsCTRL(Vector2f(1024, 633));

    Texture menuImage, menuImage2, kbBinds, ctrlBinds;
    kbBinds.loadFromFile("Assets\\Other\\kbBinds.png");
    ctrlBinds.loadFromFile("Assets\\Other\\ctrlBinds.png");
    menuKeybindsKB.setTexture(&kbBinds);
    menuKeybindsCTRL.setTexture(&ctrlBinds);

    // Has a 50/50 chance of getting each main menu style
    uiMenuStyle = rand() % 2;
    menuImage.loadFromFile("Assets\\Other\\menu.png");
    menuMusic.openFromFile("Assets\\Music\\menu.ogg");
    menuImage2.loadFromFile("Assets\\Other\\menu2.png");
    menuMusic2.openFromFile("Assets\\Music\\menu2.ogg");
    if (uiMenuStyle == 0) {
        menuAnim.setTexture(&menuImage);
    }
    else menuAnim.setTexture(&menuImage2);

    RectangleShape menuBox(Vector2f(app.getSize().x, 135));
    menuBox.setFillColor(Color::Black);
    menuBox.setPosition(Vector2f(0, app.getSize().y - 135));
    menuBox.setOutlineThickness(2);

    RectangleShape button_menuSelection1(Vector2f(200, 35));
    button_menuSelection1.setPosition(10, app.getSize().y - 125);
    button_menuSelection1.setFillColor(Color(35, 35, 35));

    Text text_menuSelection1("New/Load Game", sansRegular, 19);
    text_menuSelection1.setPosition(button_menuSelection1.getPosition().x + 10, button_menuSelection1.getPosition().y + 5);

    RectangleShape button_menuSelection2(Vector2f(200, 35));
    button_menuSelection2.setPosition(10, app.getSize().y - 84);
    button_menuSelection2.setFillColor(Color(35, 35, 35));

    Text text_menuSelection2("Controls", sansRegular, 19);
    text_menuSelection2.setPosition(button_menuSelection2.getPosition().x + 10, button_menuSelection2.getPosition().y + 5);

    RectangleShape button_menuSelection3(Vector2f(200, 35));
    button_menuSelection3.setPosition(10, app.getSize().y - 44);
    button_menuSelection3.setFillColor(Color(35, 35, 35));

    Text text_menuSelection3("Exit Game", sansRegular, 19);
    text_menuSelection3.setPosition(button_menuSelection3.getPosition().x + 10, button_menuSelection3.getPosition().y + 5);

    Text text_credit("Brandon Luu 2019", sansRegular, 16);
    text_credit.setPosition(Vector2f(app.getSize().x - 150, app.getSize().y - 25));

    Text text_menuHint("This is sample text", sansRegular, 17);
    text_menuHint.setPosition(Vector2f(235, app.getSize().y - 125));

    RectangleShape menu_char1(Vector2f(72, 100));
    menu_char1.setPosition(Vector2f(393, app.getSize().y - 115));

    RectangleShape button_char1(Vector2f(72, 100));
    button_char1.setFillColor(Color(35, 35, 35));
    button_char1.setPosition(Vector2f(390, app.getSize().y - 115));

    RectangleShape menu_char2(Vector2f(72, 100));
    menu_char2.setPosition(Vector2f(503, app.getSize().y - 115));

    RectangleShape button_char2(Vector2f(72, 100));
    button_char2.setFillColor(Color(35, 35, 35));
    button_char2.setPosition(Vector2f(500, app.getSize().y - 115));

    RectangleShape menu_char3(Vector2f(72, 100));
    menu_char3.setPosition(Vector2f(613, app.getSize().y - 115));

    RectangleShape button_char3(Vector2f(72, 100));
    button_char3.setFillColor(Color(35, 35, 35));
    button_char3.setPosition(Vector2f(610, app.getSize().y - 115));

    RectangleShape menu_char4(Vector2f(72, 100));
    menu_char4.setPosition(Vector2f(723, app.getSize().y - 115));

    RectangleShape button_char4(Vector2f(72, 100));
    button_char4.setFillColor(Color(35, 35, 35));
    button_char4.setPosition(Vector2f(720, app.getSize().y - 115));

    Texture menu_imagechar1, menu_imagechar2, menu_imagechar3, menu_imagechar4;
    menu_imagechar1.loadFromFile("Assets\\Sprites\\menuChar1.png");
    menu_imagechar2.loadFromFile("Assets\\Sprites\\menuChar2.png");
    menu_imagechar3.loadFromFile("Assets\\Sprites\\menuChar3.png");
    menu_imagechar4.loadFromFile("Assets\\Sprites\\menuChar4.png");

    menu_char1.setTexture(&menu_imagechar1);
    menu_char2.setTexture(&menu_imagechar2);
    menu_char3.setTexture(&menu_imagechar3);
    menu_char4.setTexture(&menu_imagechar4);

    // [ Dialogue and Interaction Interfaces ]
    RectangleShape ui_dialogueBox(Vector2f(app.getSize().x - 30, 135));
    ui_dialogueBox.setPosition(15, app.getSize().y - 150);
    ui_dialogueBox.setFillColor(Color::Black);
    ui_dialogueBox.setOutlineThickness(2);

    Text ui_dialogueSpeaker("Billy Bob", sansBold, 18);
    ui_dialogueSpeaker.setPosition(Vector2f(40, ui_dialogueBox.getPosition().y + 15));

    Text ui_dialogueText("I am a text box.", sansRegular, 23);
    ui_dialogueText.setPosition(Vector2f(50, ui_dialogueBox.getPosition().y + 40));

    Text ui_dialogueHint("Press [ Z ] to continue", sansRegular, 15);
    ui_dialogueHint.setPosition(Vector2f(app.getSize().x - 200, app.getSize().y - 45));

    // [ Inventory Interface ]
    RectangleShape ui_inventoryBox(Vector2f(300, app.getSize().y - 75));
    ui_inventoryBox.setPosition(25, 25);
    ui_inventoryBox.setFillColor(Color::Black);
    ui_inventoryBox.setOutlineThickness(2);

    Text ui_inventoryHeader("Inventory", sansBold, 25);
    ui_inventoryHeader.setPosition(ui_inventoryBox.getPosition().x + 15, ui_inventoryBox.getPosition().y + 15);

    Text ui_inventoryHint("Goodies.", sansRegular, 17);
    ui_inventoryHint.setPosition(ui_inventoryBox.getPosition().x + 15, ui_inventoryBox.getPosition().y + 485);

    Text ui_inventorySnacks("Snacks", sansBold, 15);
    ui_inventorySnacks.setPosition(ui_inventoryBox.getPosition().x + 15, ui_inventoryBox.getPosition().y + 65);

    Text ui_inventoryWeapons("Weapons", sansBold, 15);
    ui_inventoryWeapons.setPosition(ui_inventoryBox.getPosition().x + 15, ui_inventoryBox.getPosition().y + 215);

    Text ui_inventoryPotions("Potions", sansBold, 15);
    ui_inventoryPotions.setPosition(ui_inventoryBox.getPosition().x + 15, ui_inventoryBox.getPosition().y + 425);

    Text ui_inventoryStats("Stats", sansBold, 15);
    ui_inventoryStats.setPosition(ui_inventoryBox.getPosition().x + 15, ui_inventoryBox.getPosition().y + 583);

    InventoryItem ui_inventoryApple("Apple", "consumable", "Assets\\Sprites\\apple.png", 40, 85);
    InventoryItem ui_inventorySpcApple("Special Apple", "consumable", "Assets\\Sprites\\betterapple.png", 40, 115);
    InventoryItem ui_inventoryMeat("Meat", "consumable", "Assets\\Sprites\\meat.png", 40, 145);
    InventoryItem ui_inventoryHerb("Herb", "consumable", "Assets\\Sprites\\herb.png", 40, 175);
    InventoryItem ui_inventorySword("Sword", "weapon", "Assets\\Sprites\\sword.png", 40, 235);
    InventoryItem ui_inventorySuperSword("Eyelander", "weapon", "Assets\\Sprites\\heavysword.png", 40, 265);
    InventoryItem ui_inventorySpear("Spear", "weapon", "Assets\\Sprites\\spear.png", 40, 295);
    InventoryItem ui_inventoryStaff("Magic Staff", "weapon", "Assets\\Sprites\\staff.png", 40, 325);
    InventoryItem ui_inventoryRevolver("Revolver", "weapon", "Assets\\Sprites\\revolver.png", 40, 355);
    InventoryItem ui_inventoryCrossbow("Crossbow", "weapon", "Assets\\Sprites\\crossbow.png", 40, 385);
    InventoryItem ui_inventoryHPPotion("Health Potion", "consumable", "Assets\\Sprites\\hpotion.png", 40, 445);
    uiInventory.push_back(ui_inventoryApple);
    uiInventory.push_back(ui_inventorySpcApple);
    uiInventory.push_back(ui_inventoryMeat);
    uiInventory.push_back(ui_inventoryHerb);
    uiInventory.push_back(ui_inventorySword);
    uiInventory.push_back(ui_inventorySuperSword);
    uiInventory.push_back(ui_inventorySpear);
    uiInventory.push_back(ui_inventoryStaff);
    uiInventory.push_back(ui_inventoryRevolver);
    uiInventory.push_back(ui_inventoryCrossbow);
    uiInventory.push_back(ui_inventoryHPPotion);

    InventoryItem ui_inventoryHealth("Health", "consumable", "Assets\\Sprites\\heart.png", 40, 603);
    InventoryItem ui_inventoryGold("Gold", "consumable", "Assets\\Sprites\\gold.png", 40, 633);
    uiInventory.push_back(ui_inventoryHealth);
    uiInventory.push_back(ui_inventoryGold);

    // [ Combat Interface ]
    Texture encounter, winterBackground;
    encounter.loadFromFile("Assets\\Sprites\\alert.png");
    winterBackground.loadFromFile("Assets\\Sprites\\winterBack.png");
    RectangleShape encounterBox(Vector2f(20, 20));
    encounterBox.setPosition(app.getSize().x/2 - 10, app.getSize().y/2 - 50);
    encounterBox.setTexture(&encounter);
    RectangleShape winterBack(Vector2f(1024, 633));
    winterBack.setFillColor(Color(0, 0, 0, 0));
    winterBack.setTexture(&winterBackground);

    RectangleShape button_combatSelection1(Vector2f(300, 35));
    button_combatSelection1.setPosition(10, app.getSize().y - 125);
    button_combatSelection1.setFillColor(Color(35, 35, 35));

    Text text_combatSelection1("Fight!", sansRegular, 19);
    text_combatSelection1.setPosition(button_combatSelection1.getPosition().x + 10, button_combatSelection1.getPosition().y + 5);

    RectangleShape button_combatSelection2(Vector2f(200, 35));
    button_combatSelection2.setPosition(10, app.getSize().y - 84);
    button_combatSelection2.setFillColor(Color(35, 35, 35));

    Text text_combatSelection2("Item", sansRegular, 19);
    text_combatSelection2.setPosition(button_combatSelection2.getPosition().x + 10, button_combatSelection2.getPosition().y + 5);

    RectangleShape button_combatSelection3(Vector2f(200, 35));
    button_combatSelection3.setPosition(10, app.getSize().y - 44);
    button_combatSelection3.setFillColor(Color(35, 35, 35));

    Text text_combatSelection3("Flee", sansRegular, 19);
    text_combatSelection3.setPosition(button_combatSelection3.getPosition().x + 10, button_combatSelection3.getPosition().y + 5);

    Text text_combatHint("I'm a combat hint.", sansRegular, 17);
    text_combatHint.setPosition(250, app.getSize().y - 35);

    Text text_combatInvTitle("Item", sansBold, 25);
    text_combatInvTitle.setPosition(15, app.getSize().y - 125);

    Text text_combatEsc("[ TAB ] Return to combat", sansRegular, 17);
    text_combatEsc.setPosition(15, app.getSize().y - 95);

    Text text_combatWeapon("Weapon: Your Bare Fists", sansRegular, 17);
    text_combatWeapon.setPosition(15, app.getSize().y - 50);

    Text text_combatInventoryHint("I'm an inventory hint, thingy. (-999HP)", sansRegular, 17);
    text_combatInventoryHint.setPosition(550, app.getSize().y - 37);

    InventoryItem ui_combatApple("Apple", "consumable", "Assets\\Sprites\\apple.png", 250, app.getSize().y - 145);
    InventoryItem ui_combatSpcApple("Special Apple", "consumable", "Assets\\Sprites\\betterapple.png", 250, app.getSize().y - 105);
    InventoryItem ui_combatMeat("Meat", "consumable", "Assets\\Sprites\\meat.png", 250, app.getSize().y - 65);
    InventoryItem ui_combatHerb("Herb", "consumable", "Assets\\Sprites\\herb.png", 550, app.getSize().y - 145);
    InventoryItem ui_combatHPPotion("Health Potion", "consumable", "Assets\\Sprites\\hpotion.png", 550, app.getSize().y - 105);
    combatInventory.push_back(ui_combatApple);
    combatInventory.push_back(ui_combatSpcApple);
    combatInventory.push_back(ui_combatMeat);
    combatInventory.push_back(ui_combatHerb);
    combatInventory.push_back(ui_combatHPPotion);

    Text text_combatFightTitle("Fight!", sansBold, 25);
    text_combatFightTitle.setPosition(15, app.getSize().y - 125);

    RectangleShape button_combatFightMove1(Vector2f(300, 35));
    button_combatFightMove1.setPosition(250, app.getSize().y - 125);
    button_combatFightMove1.setFillColor(Color(35, 35, 35));

    Text text_combatFightMove1("Move 1", sansRegular, 19);
    text_combatFightMove1.setPosition(button_combatFightMove1.getPosition().x + 10, button_combatFightMove1.getPosition().y + 5);

    RectangleShape button_combatFightMove2(Vector2f(300, 35));
    button_combatFightMove2.setPosition(250, app.getSize().y - 84);
    button_combatFightMove2.setFillColor(Color(35, 35, 35));

    Text text_combatFightMove2("Move 2", sansRegular, 19);
    text_combatFightMove2.setPosition(button_combatFightMove2.getPosition().x + 10, button_combatFightMove2.getPosition().y + 5);

    RectangleShape button_combatFightMove3(Vector2f(300, 35));
    button_combatFightMove3.setPosition(250, app.getSize().y - 44);
    button_combatFightMove3.setFillColor(Color(35, 35, 35));

    Text text_combatFightMove3("Move 3", sansRegular, 19);
    text_combatFightMove3.setPosition(button_combatFightMove3.getPosition().x + 10, button_combatFightMove3.getPosition().y + 5);

    RectangleShape ui_combatPlayerTag(Vector2f(335, 30));
    ui_combatPlayerTag.setPosition(Vector2f(25, app.getSize().y - 175));
    ui_combatPlayerTag.setFillColor(Color(0, 0, 0));
    ui_combatPlayerTag.setOutlineThickness(2);

    Text text_combatPlayerTag("You", sansRegular, 20);
    text_combatPlayerTag.setPosition(ui_combatPlayerTag.getPosition().x + 10, ui_combatPlayerTag.getPosition().y + 2);

    InventoryItem ui_combatPlayerHealth("Health", "consumable", "Assets\\Sprites\\heart.png", ui_combatPlayerTag.getPosition().x + 65, app.getSize().y - 200);

    RectangleShape ui_combatEnemyTag(Vector2f(375, 30));
    ui_combatEnemyTag.setPosition(Vector2f(app.getSize().x - 400, app.getSize().y - 175));
    ui_combatEnemyTag.setFillColor(Color(0, 0, 0));
    ui_combatEnemyTag.setOutlineThickness(2);

    Text text_combatEnemyTag("Enemy", sansRegular, 20);
    text_combatEnemyTag.setPosition(ui_combatEnemyTag.getPosition().x + 280, ui_combatEnemyTag.getPosition().y + 2);

    InventoryItem ui_combatEnemyHealth("Health", "consumable", "Assets\\Sprites\\heart.png", ui_combatEnemyTag.getPosition().x, app.getSize().y - 200);

    Text text_combatDamageAmount("0", sansRegular, 40);

    Text ui_combatText("I am a text box.", sansRegular, 23);
    ui_combatText.setPosition(Vector2f(50, ui_dialogueBox.getPosition().y + 40));

    // [ Status Bar and Tutorial Thing]
    Texture generalHelp;
    generalHelp.loadFromFile("Assets\\Other\\help.png");
    RectangleShape ui_GeneralHelp(Vector2f(1024, 768));
    ui_GeneralHelp.setTexture(&generalHelp);
    Text text_helpHealth("999", sansRegular, 17);
    text_helpHealth.setPosition(Vector2f(792, 10));
    Text text_helpGold("999", sansRegular, 17);
    text_helpGold.setPosition(Vector2f(865, 10));

    // [ Shop ]
    RectangleShape ui_shopBox(Vector2f(300, 280));
    ui_shopBox.setPosition(Vector2f(app.getSize().x - 315, menuBox.getPosition().y - 310));
    ui_shopBox.setFillColor(Color(0, 0, 0));
    ui_shopBox.setOutlineThickness(2);

    Text text_shopHeader("Shop", sansBold, 25);
    text_shopHeader.setPosition(ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 15);

    Text text_shopExit("[ TAB ] Exit Shop", sansRegular, 16);
    text_shopExit.setPosition(Vector2f(ui_shopBox.getPosition().x + 150, ui_shopBox.getPosition().y + 19));

    InventoryItem ui_shopApple("Apple", "weapon", "Assets\\Sprites\\apple.png", ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 30);
    InventoryItem ui_shopSpcApple("Special Apple", "weapon", "Assets\\Sprites\\betterapple.png", ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 60);
    InventoryItem ui_shopMeat("Meat", "weapon", "Assets\\Sprites\\meat.png", ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 90);
    InventoryItem ui_shopHerb("Herb", "weapon", "Assets\\Sprites\\herb.png", ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 120);
    InventoryItem ui_shopHPPotion("Health Potion", "weapon", "Assets\\Sprites\\hpotion.png", ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 150);
    InventoryItem ui_shopGold("Gold", "weapon", "Assets\\Sprites\\gold.png", ui_shopBox.getPosition().x + 15, ui_shopBox.getPosition().y + 210);
    shopInventory.push_back(ui_shopApple);
    shopInventory.push_back(ui_shopSpcApple);
    shopInventory.push_back(ui_shopMeat);
    shopInventory.push_back(ui_shopHerb);
    shopInventory.push_back(ui_shopHPPotion);
    shopInventory.push_back(ui_shopGold);

    // [ Main Sprites ]
    progressLoading(app, text_loadingText, "[ Loading Sprites ]", loadWheel);
    Texture tx_player; tx_player.loadFromFile("Assets\\Sprites\\player.png");
    Sprite spr_player(tx_player, IntRect(0, 0, 26, 36));
    spr_player.setScale(2, 2);
    spr_player.setPosition(PlayerFile["player_x"], PlayerFile["player_y"]);
    mainView.setCenter(PlayerFile["view_x"], PlayerFile["view_y"]);

    // area 1
    NPC spr_guard1("guard1", Vector2f(3474, 7539), "Guard", "Assets\\Sprites\\guard1.png"); spr_npcs.push_back(spr_guard1);
    NPC spr_guard2("guard2", Vector2f(3860, 6426), "Billy", "Assets\\Sprites\\guard1.png"); spr_npcs.push_back(spr_guard2);
    NPC spr_shopkeeper1("shopkeeper1", Vector2f(3498, 6954), "Hannah (Shopkeeper)", "Assets\\Sprites\\shopkeeper.png"); spr_npcs.push_back(spr_shopkeeper1);
    NPC spr_healer1("healer1", Vector2f(4478, 7302), "Maria (Healer)", "Assets\\Sprites\\healer.png"); spr_npcs.push_back(spr_healer1);
    NPC spr_npc1a1("npc1a1", Vector2f(4546, 7425), "Spencer", "Assets\\Sprites\\npc1a1.png"); spr_npcs.push_back(spr_npc1a1);
    NPC spr_npc2a1("npc2a1", Vector2f(4509, 6974), "Barry", "Assets\\Sprites\\npc2a1.png"); spr_npcs.push_back(spr_npc2a1);
    NPC spr_npc3a1("npc3a1", Vector2f(3929, 6747), "Kareem", "Assets\\Sprites\\npc3a1.png"); spr_npcs.push_back(spr_npc3a1);
    NPC spr_npc4a1("npc4a1", Vector2f(3982, 6747), "Paul", "Assets\\Sprites\\npc4a1.png"); spr_npcs.push_back(spr_npc4a1);
    NPC spr_npc5a1("npc5a1", Vector2f(3658, 7511), "Jake", "Assets\\Sprites\\npc5a1.png"); spr_npcs.push_back(spr_npc5a1);
    NPC spr_guard3("guard3", Vector2f(3986, 7400), "Gezequel", "Assets\\Sprites\\guard1.png"); spr_npcs.push_back(spr_guard3);

    // area 2
    NPC spr_shopkeeper2("shopkeeper2", Vector2f(2013, 2015), "Maggie (Shopkeeper)", "Assets\\Sprites\\shopkeeper2.png"); spr_npcs.push_back(spr_shopkeeper2);
    NPC spr_healer2("healer2", Vector2f(1804, 2340), "Erik (Healer)", "Assets\\Sprites\\healer2.png"); spr_npcs.push_back(spr_healer2);
    NPC spr_npc1a2("npc1a2", Vector2f(2652, 2185), "Rackman", "Assets\\Sprites\\npc1a2.png"); spr_npcs.push_back(spr_npc1a2);
    NPC spr_npc2a2("npc2a2", Vector2f(2456, 2677), "Link", "Assets\\Sprites\\npc2a2.png"); spr_npcs.push_back(spr_npc2a2);
    NPC spr_npc3a2("npc3a2", Vector2f(1748, 1695), "Skeletor", "Assets\\Sprites\\npc3a2.png"); spr_npcs.push_back(spr_npc3a2);
    NPC spr_npc4a2("npc4a2", Vector2f(2670, 1677), "Valkyrie", "Assets\\Sprites\\npc4a2.png"); spr_npcs.push_back(spr_npc4a2);
    NPC spr_npc5a2("npc5a2", Vector2f(2860, 1868), "Valorie", "Assets\\Sprites\\npc5a2.png"); spr_npcs.push_back(spr_npc5a2);

    // area 3
    NPC spr_shopkeeper3("shopkeeper3", Vector2f(7206, 2338), "Selo (Shopkeeper)", "Assets\\Sprites\\shopkeeper3.png"); spr_npcs.push_back(spr_shopkeeper3);
    NPC spr_healer3("healer3", Vector2f(6375, 2010), "Jacob (Healer)", "Assets\\Sprites\\healer3.png"); spr_npcs.push_back(spr_healer3);
    NPC spr_npc1a3("npc1a3", Vector2f(6382, 2357), "Kenzie", "Assets\\Sprites\\npc1a3.png"); spr_npcs.push_back(spr_npc1a3);
    NPC spr_npc2a3("npc2a3", Vector2f(7039, 2858), "Roger", "Assets\\Sprites\\npc2a3.png"); spr_npcs.push_back(spr_npc2a3);
    NPC spr_npc3a3("npc3a3", Vector2f(6672, 2776), "Vikter", "Assets\\Sprites\\npc3a3.png"); spr_npcs.push_back(spr_npc3a3);
    NPC spr_npc4a3("npc4a3", Vector2f(6720, 2776), "Brayden", "Assets\\Sprites\\npc4a3.png"); spr_npcs.push_back(spr_npc4a3);

    NPC enemy("enemy", Vector2f(500, 300), "Johnny Johnny", "Assets\\Sprites\\raptor1.png"); // Enemy for combat
    Texture raptor1, raptor2, bird1, wolf1, wolf2;
    raptor1.loadFromFile("Assets\\Sprites\\raptor1.png");
    raptor2.loadFromFile("Assets\\Sprites\\raptor2.png");
    bird1.loadFromFile("Assets\\Sprites\\bird1.png");
    wolf1.loadFromFile("Assets\\Sprites\\wolf1.png");
    wolf2.loadFromFile("Assets\\Sprites\\wolf2.png");
    int enemyHP = 100;

    // [ Combat Effects ]
    progressLoading(app, text_loadingText, "[ Loading Effects ]", loadWheel);
    RectangleShape combatEffect; // Combat effect sprites
    combatEffect.setSize(Vector2f(228, 83));
    Texture sword1tex, sword2tex, sword3tex, eyelander1tex, eyelander2tex, eyelander3tex, spear1tex, spear2tex, spear3tex, healtex;
    Texture staff1tex, staff2tex, staff3tex, revolver1tex, revolver2tex, revolver3tex, crossbow1tex, crossbow2tex, crossbow3tex;
    sword1tex.loadFromFile("Assets\\Sprites\\sword1.png");
    sword2tex.loadFromFile("Assets\\Sprites\\sword2.png");
    sword3tex.loadFromFile("Assets\\Sprites\\sword3.png");
    eyelander1tex.loadFromFile("Assets\\Sprites\\eyelander1.png");
    eyelander2tex.loadFromFile("Assets\\Sprites\\eyelander2.png");
    eyelander3tex.loadFromFile("Assets\\Sprites\\eyelander3.png");
    spear1tex.loadFromFile("Assets\\Sprites\\spear1.png");
    spear2tex.loadFromFile("Assets\\Sprites\\spear2.png");
    spear3tex.loadFromFile("Assets\\Sprites\\spear3.png");
    staff1tex.loadFromFile("Assets\\Sprites\\staff1.png");
    staff2tex.loadFromFile("Assets\\Sprites\\staff2.png");
    staff3tex.loadFromFile("Assets\\Sprites\\staff3.png");
    revolver1tex.loadFromFile("Assets\\Sprites\\revolver1.png");
    revolver2tex.loadFromFile("Assets\\Sprites\\revolver2.png");
    revolver3tex.loadFromFile("Assets\\Sprites\\revolver3.png");
    crossbow1tex.loadFromFile("Assets\\Sprites\\crossbow1.png");
    crossbow2tex.loadFromFile("Assets\\Sprites\\crossbow2.png");
    crossbow3tex.loadFromFile("Assets\\Sprites\\crossbow3.png");
    healtex.loadFromFile("Assets\\Sprites\\heal.png");

    // Map
    progressLoading(app, text_loadingText, "[ Loading Map ]", loadWheel);
    Sprite dimerMap;
    Texture dimerTex;
    dimerTex.loadFromFile("Assets\\Sprites\\dimer2.jpg"); // massive map, at the maximum render size (4096, 4096)
    dimerMap.setTexture(dimerTex);
    dimerMap.setScale(2, 2);
    dimerMap.setPosition(Vector2f(15, 25));

    // Map barrier creation
    vector<RectangleShape> barrier;
    barrier.push_back(RectangleShape(Vector2f(1136.64,552.96)));
    barrier.back().setPosition(2775.04,7700.48);
    barrier.push_back(RectangleShape(Vector2f(1044.48,686.08)));
    barrier.back().setPosition(4249.6,7720.96);
    barrier.push_back(RectangleShape(Vector2f(-481.28,1894.4)));
    barrier.back().setPosition(5273.6,5826.56);
    barrier.push_back(RectangleShape(Vector2f(512,1863.68)));
    barrier.back().setPosition(2764.8,5836.8);
    barrier.push_back(RectangleShape(Vector2f(419.84,358.4)));
    barrier.back().setPosition(3307.52,6604.8);
    barrier.push_back(RectangleShape(Vector2f(481.28,409.6)));
    barrier.back().setPosition(4280.32,6574.08);
    barrier.push_back(RectangleShape(Vector2f(153.6,163.84)));
    barrier.back().setPosition(4474.88,7147.52);
    barrier.push_back(RectangleShape(Vector2f(235.52,460.8)));
    barrier.back().setPosition(3379.2,7075.84);
    barrier.push_back(RectangleShape(Vector2f(92.16,51.2)));
    barrier.back().setPosition(3624.96,7464.96);
    barrier.push_back(RectangleShape(Vector2f(143.36,153.6)));
    barrier.back().setPosition(3932.16,7239.68);
    barrier.push_back(RectangleShape(Vector2f(112.64,133.12)));
    barrier.back().setPosition(3993.6,7157.76);
    barrier.push_back(RectangleShape(Vector2f(409.6,358.4)));
    barrier.back().setPosition(1822.72,1669.12);
    barrier.push_back(RectangleShape(Vector2f(143.36,153.6)));
    barrier.back().setPosition(1740.8,2191.36);
    barrier.push_back(RectangleShape(Vector2f(440.32,399.36)));
    barrier.back().setPosition(2498.56,2293.76);
    barrier.push_back(RectangleShape(Vector2f(51.2,81.92)));
    barrier.back().setPosition(2447.36,2600.96);
    barrier.push_back(RectangleShape(Vector2f(51.2,81.92)));
    barrier.back().setPosition(2938.88,2600.96);
    barrier.push_back(RectangleShape(Vector2f(133.12,92.16)));
    barrier.back().setPosition(2600.96,2099.2);
    barrier.push_back(RectangleShape(Vector2f(102.4,61.44)));
    barrier.back().setPosition(2170.88,2539.52);
    barrier.push_back(RectangleShape(Vector2f(112.64,153.6)));
    barrier.back().setPosition(2263.04,2058.24);
    barrier.push_back(RectangleShape(Vector2f(102.4,122.88)));
    barrier.back().setPosition(2314.24,1996.8);
    barrier.push_back(RectangleShape(Vector2f(409.6,348.16)));
    barrier.back().setPosition(7014.4,1996.8);
    barrier.push_back(RectangleShape(Vector2f(235.52,491.52)));
    barrier.back().setPosition(7096.32,2478.08);
    barrier.push_back(RectangleShape(Vector2f(71.68,51.2)));
    barrier.back().setPosition(7362.56,2887.68);
    barrier.push_back(RectangleShape(Vector2f(102.4,112.64)));
    barrier.back().setPosition(6174.72,2570.24);
    barrier.push_back(RectangleShape(Vector2f(61.44,102.4)));
    barrier.back().setPosition(6307.84,2324.48);
    barrier.push_back(RectangleShape(Vector2f(153.6,174.08)));
    barrier.back().setPosition(6348.8,1853.44);
    barrier.push_back(RectangleShape(Vector2f(51.2,92.16)));
    barrier.back().setPosition(6225.92,1853.44);
    barrier.push_back(RectangleShape(Vector2f(225.28,225.28)));
    barrier.back().setPosition(3870.72,6184.96);
    barrier.push_back(RectangleShape(Vector2f(-1146.88,-645.12)));
    barrier.back().setPosition(5089.28,235.52);
    barrier.push_back(RectangleShape(Vector2f(952.32,276.48)));
    barrier.back().setPosition(5058.56,61.44);
    barrier.push_back(RectangleShape(Vector2f(737.28,81.92)));
    barrier.back().setPosition(5263.36,327.68);
    barrier.push_back(RectangleShape(Vector2f(788.48,266.24)));
    barrier.back().setPosition(6993.92,1648.64);
    barrier.push_back(RectangleShape(Vector2f(235.52,1249.28)));
    barrier.back().setPosition(7557.12,1884.16);
    barrier.push_back(RectangleShape(Vector2f(880.64,266.24)));
    barrier.back().setPosition(5816.32,1607.68);
    barrier.push_back(RectangleShape(Vector2f(256,1177.6)));
    barrier.back().setPosition(5826.56,1781.76);
    barrier.push_back(RectangleShape(Vector2f(204.8,276.48)));
    barrier.back().setPosition(5959.68,2846.72);
    barrier.push_back(RectangleShape(Vector2f(348.16,256)));
    barrier.back().setPosition(6133.76,3000.32);
    barrier.push_back(RectangleShape(Vector2f(716.8,266.24)));
    barrier.back().setPosition(6952.96,3051.52);
    barrier.push_back(RectangleShape(Vector2f(225.28,716.8)));
    barrier.back().setPosition(6328.32,3225.6);
    barrier.push_back(RectangleShape(Vector2f(215.04,942.08)));
    barrier.back().setPosition(6871.04,3256.32);
    barrier.push_back(RectangleShape(Vector2f(706.56,235.52)));
    barrier.back().setPosition(1075.2,2734.08);
    barrier.push_back(RectangleShape(Vector2f(348.16,1423.36)));
    barrier.back().setPosition(1351.68,1413.12);
    barrier.push_back(RectangleShape(Vector2f(1218.56,235.52)));
    barrier.back().setPosition(1556.48,1402.88);
    barrier.push_back(RectangleShape(Vector2f(419.84,235.52)));
    barrier.back().setPosition(1720.32,2805.76);
    barrier.push_back(RectangleShape(Vector2f(-266.24,245.76)));
    barrier.back().setPosition(3297.28,2744.32);
    barrier.push_back(RectangleShape(Vector2f(-727.04,235.52)));
    barrier.back().setPosition(3072,2826.24);
    barrier.push_back(RectangleShape(Vector2f(768,593.92)));
    barrier.back().setPosition(5580.8,3696.64);
    barrier.push_back(RectangleShape(Vector2f(153.6,245.76)));
    barrier.back().setPosition(6307.84,3901.44);
    barrier.push_back(RectangleShape(Vector2f(593.92,143.36)));
    barrier.back().setPosition(5642.24,4270.08);
    barrier.push_back(RectangleShape(Vector2f(491.52,245.76)));
    barrier.back().setPosition(5160.96,3522.56);
    barrier.push_back(RectangleShape(Vector2f(153.6,225.28)));
    barrier.back().setPosition(6789.12,4147.2);
    barrier.push_back(RectangleShape(Vector2f(194.56,235.52)));
    barrier.back().setPosition(6645.76,4321.28);
    barrier.push_back(RectangleShape(Vector2f(184.32,245.76)));
    barrier.back().setPosition(6533.12,4454.4);
    barrier.push_back(RectangleShape(Vector2f(153.6,235.52)));
    barrier.back().setPosition(6400,4526.08);
    barrier.push_back(RectangleShape(Vector2f(163.84,225.28)));
    barrier.back().setPosition(6236.16,4597.76);
    barrier.push_back(RectangleShape(Vector2f(645.12,286.72)));
    barrier.back().setPosition(5591.04,4618.24);
    barrier.push_back(RectangleShape(Vector2f(153.6,266.24)));
    barrier.back().setPosition(5457.92,4546.56);
    barrier.push_back(RectangleShape(Vector2f(174.08,286.72)));
    barrier.back().setPosition(5294.08,4454.4);
    barrier.push_back(RectangleShape(Vector2f(276.48,286.72)));
    barrier.back().setPosition(5058.56,4352);
    barrier.push_back(RectangleShape(Vector2f(184.32,245.76)));
    barrier.back().setPosition(4915.2,4321.28);
    barrier.push_back(RectangleShape(Vector2f(194.56,245.76)));
    barrier.back().setPosition(4741.12,4259.84);
    barrier.push_back(RectangleShape(Vector2f(174.08,215.04)));
    barrier.back().setPosition(4720.64,4474.88);
    barrier.push_back(RectangleShape(Vector2f(174.08,256)));
    barrier.back().setPosition(4761.6,4659.2);
    barrier.push_back(RectangleShape(Vector2f(215.04,1003.52)));
    barrier.back().setPosition(4843.52,4874.24);
    barrier.push_back(RectangleShape(Vector2f(368.64,194.56)));
    barrier.back().setPosition(4833.28,3594.24);
    barrier.push_back(RectangleShape(Vector2f(174.08,215.04)));
    barrier.back().setPosition(4659.2,3655.68);
    barrier.push_back(RectangleShape(Vector2f(204.8,225.28)));
    barrier.back().setPosition(4515.84,3829.76);
    barrier.push_back(RectangleShape(Vector2f(266.24,368.64)));
    barrier.back().setPosition(4300.8,3973.12);
    barrier.push_back(RectangleShape(Vector2f(184.32,1146.88)));
    barrier.back().setPosition(4321.28,4290.56);
    barrier.push_back(RectangleShape(Vector2f(153.6,174.08)));
    barrier.back().setPosition(4311.04,5447.68);
    barrier.push_back(RectangleShape(Vector2f(143.36,122.88)));
    barrier.back().setPosition(4259.84,5611.52);
    barrier.push_back(RectangleShape(Vector2f(696.32,133.12)));
    barrier.back().setPosition(3655.68,5754.88);
    barrier.push_back(RectangleShape(Vector2f(593.92,163.84)));
    barrier.back().setPosition(3696.64,5857.28);
    barrier.push_back(RectangleShape(Vector2f(532.48,174.08)));
    barrier.back().setPosition(3717.12,6000.64);
    barrier.push_back(RectangleShape(Vector2f(460.8,194.56)));
    barrier.back().setPosition(3758.08,6144);
    barrier.push_back(RectangleShape(Vector2f(235.52,266.24)));
    barrier.back().setPosition(3543.04,5498.88);
    barrier.push_back(RectangleShape(Vector2f(2263.04,215.04)));
    barrier.back().setPosition(3870.72,788.48);
    barrier.push_back(RectangleShape(Vector2f(102.4,133.12)));
    barrier.back().setPosition(3983.36,675.84);
    barrier.push_back(RectangleShape(Vector2f(716.8,184.32)));
    barrier.back().setPosition(5324.8,604.16);
    barrier.push_back(RectangleShape(Vector2f(163.84,225.28)));
    barrier.back().setPosition(6123.52,890.88);
    barrier.push_back(RectangleShape(Vector2f(184.32,225.28)));
    barrier.back().setPosition(6195.2,1034.24);
    barrier.push_back(RectangleShape(Vector2f(153.6,245.76)));
    barrier.back().setPosition(6318.08,1187.84);
    barrier.push_back(RectangleShape(Vector2f(163.84,245.76)));
    barrier.back().setPosition(6430.72,1269.76);
    barrier.push_back(RectangleShape(Vector2f(163.84,225.28)));
    barrier.back().setPosition(6512.64,1423.36);
    barrier.push_back(RectangleShape(Vector2f(143.36,225.28)));
    barrier.back().setPosition(6973.44,1464.32);
    barrier.push_back(RectangleShape(Vector2f(163.84,256)));
    barrier.back().setPosition(6922.24,1239.04);
    barrier.push_back(RectangleShape(Vector2f(215.04,235.52)));
    barrier.back().setPosition(6809.6,1064.96);
    barrier.push_back(RectangleShape(Vector2f(215.04,245.76)));
    barrier.back().setPosition(6686.72,860.16);
    barrier.push_back(RectangleShape(Vector2f(153.6,184.32)));
    barrier.back().setPosition(6502.4,696.32);
    barrier.push_back(RectangleShape(Vector2f(153.6,194.56)));
    barrier.back().setPosition(6584.32,808.96);
    barrier.push_back(RectangleShape(Vector2f(153.6,235.52)));
    barrier.back().setPosition(6410.24,532.48);
    barrier.push_back(RectangleShape(Vector2f(153.6,235.52)));
    barrier.back().setPosition(6277.12,430.08);
    barrier.push_back(RectangleShape(Vector2f(163.84,235.52)));
    barrier.back().setPosition(6133.76,368.64);
    barrier.push_back(RectangleShape(Vector2f(174.08,256)));
    barrier.back().setPosition(5980.16,286.72);
    barrier.push_back(RectangleShape(Vector2f(112.64,163.84)));
    barrier.back().setPosition(3102.72,5693.44);
    barrier.push_back(RectangleShape(Vector2f(133.12,194.56)));
    barrier.back().setPosition(3031.04,5509.12);
    barrier.push_back(RectangleShape(Vector2f(153.6,174.08)));
    barrier.back().setPosition(3799.04,122.88);
    barrier.push_back(RectangleShape(Vector2f(174.08,133.12)));
    barrier.back().setPosition(3686.4,245.76);
    barrier.push_back(RectangleShape(Vector2f(143.36,153.6)));
    barrier.back().setPosition(3604.48,419.84);
    barrier.push_back(RectangleShape(Vector2f(51.2,51.2)));
    barrier.back().setPosition(3676.16,378.88);
    barrier.push_back(RectangleShape(Vector2f(122.88,174.08)));
    barrier.back().setPosition(3512.32,542.72);
    barrier.push_back(RectangleShape(Vector2f(143.36,163.84)));
    barrier.back().setPosition(3389.44,675.84);
    barrier.push_back(RectangleShape(Vector2f(143.36,174.08)));
    barrier.back().setPosition(3266.56,808.96);
    barrier.push_back(RectangleShape(Vector2f(163.84,194.56)));
    barrier.back().setPosition(3174.4,952.32);
    barrier.push_back(RectangleShape(Vector2f(122.88,194.56)));
    barrier.back().setPosition(3082.24,1064.96);
    barrier.push_back(RectangleShape(Vector2f(153.6,184.32)));
    barrier.back().setPosition(2959.36,1167.36);
    barrier.push_back(RectangleShape(Vector2f(163.84,174.08)));
    barrier.back().setPosition(2856.96,1280);
    barrier.push_back(RectangleShape(Vector2f(153.6,153.6)));
    barrier.back().setPosition(2744.32,1392.64);
    barrier.push_back(RectangleShape(Vector2f(143.36,215.04)));
    barrier.back().setPosition(3706.88,911.36);
    barrier.push_back(RectangleShape(Vector2f(112.64,122.88)));
    barrier.back().setPosition(3778.56,880.64);
    barrier.push_back(RectangleShape(Vector2f(112.64,194.56)));
    barrier.back().setPosition(3584,983.04);
    barrier.push_back(RectangleShape(Vector2f(153.6,194.56)));
    barrier.back().setPosition(3450.88,1136.64);
    barrier.push_back(RectangleShape(Vector2f(133.12,225.28)));
    barrier.back().setPosition(3368.96,1280);
    barrier.push_back(RectangleShape(Vector2f(143.36,204.8)));
    barrier.back().setPosition(3256.32,1402.88);
    barrier.push_back(RectangleShape(Vector2f(143.36,204.8)));
    barrier.back().setPosition(3153.92,1536);
    barrier.push_back(RectangleShape(Vector2f(112.64,163.84)));
    barrier.back().setPosition(3072,1648.64);
    barrier.push_back(RectangleShape(Vector2f(143.36,153.6)));
    barrier.back().setPosition(3000.32,1761.28);
    barrier.push_back(RectangleShape(Vector2f(163.84,276.48)));
    barrier.back().setPosition(2918.4,1894.4);
    barrier.push_back(RectangleShape(Vector2f(143.36,174.08)));
    barrier.back().setPosition(2969.6,2160.64);
    barrier.push_back(RectangleShape(Vector2f(204.8,460.8)));
    barrier.back().setPosition(3020.8,2314.24);
    barrier.push_back(RectangleShape(Vector2f(133.12,163.84)));
    barrier.back().setPosition(1955.84,3051.52);
    barrier.push_back(RectangleShape(Vector2f(163.84,153.6)));
    barrier.back().setPosition(1884.16,3225.6);
    barrier.push_back(RectangleShape(Vector2f(122.88,143.36)));
    barrier.back().setPosition(1853.44,3389.44);
    barrier.push_back(RectangleShape(Vector2f(225.28,92.16)));
    barrier.back().setPosition(1699.84,3522.56);
    barrier.push_back(RectangleShape(Vector2f(348.16,81.92)));
    barrier.back().setPosition(1536,3604.48);
    barrier.push_back(RectangleShape(Vector2f(133.12,143.36)));
    barrier.back().setPosition(1484.8,3665.92);
    barrier.push_back(RectangleShape(Vector2f(133.12,225.28)));
    barrier.back().setPosition(1382.4,3860.48);
    barrier.push_back(RectangleShape(Vector2f(122.88,153.6)));
    barrier.back().setPosition(1433.6,3758.08);
    barrier.push_back(RectangleShape(Vector2f(153.6,286.72)));
    barrier.back().setPosition(1331.2,4065.28);
    barrier.push_back(RectangleShape(Vector2f(143.36,163.84)));
    barrier.back().setPosition(1423.36,4280.32);
    barrier.push_back(RectangleShape(Vector2f(122.88,174.08)));
    barrier.back().setPosition(1536,4362.24);
    barrier.push_back(RectangleShape(Vector2f(163.84,296.96)));
    barrier.back().setPosition(1587.2,4505.6);
    barrier.push_back(RectangleShape(Vector2f(174.08,286.72)));
    barrier.back().setPosition(1638.4,4771.84);
    barrier.push_back(RectangleShape(Vector2f(204.8,235.52)));
    barrier.back().setPosition(1792,4945.92);
    barrier.push_back(RectangleShape(Vector2f(194.56,194.56)));
    barrier.back().setPosition(1976.32,5038.08);
    barrier.push_back(RectangleShape(Vector2f(266.24,194.56)));
    barrier.back().setPosition(2140.16,5109.76);
    barrier.push_back(RectangleShape(Vector2f(225.28,235.52)));
    barrier.back().setPosition(2396.16,5191.68);
    barrier.push_back(RectangleShape(Vector2f(184.32,215.04)));
    barrier.back().setPosition(2621.44,5304.32);
    barrier.push_back(RectangleShape(Vector2f(256,204.8)));
    barrier.back().setPosition(2795.52,5355.52);
    barrier.push_back(RectangleShape(Vector2f(133.12,215.04)));
    barrier.back().setPosition(2334.72,3051.52);
    barrier.push_back(RectangleShape(Vector2f(153.6,184.32)));
    barrier.back().setPosition(2293.76,3235.84);
    barrier.push_back(RectangleShape(Vector2f(163.84,153.6)));
    barrier.back().setPosition(2242.56,3420.16);
    barrier.push_back(RectangleShape(Vector2f(143.36,122.88)));
    barrier.back().setPosition(2211.84,3573.76);
    barrier.push_back(RectangleShape(Vector2f(163.84,153.6)));
    barrier.back().setPosition(2119.68,3665.92);
    barrier.push_back(RectangleShape(Vector2f(133.12,163.84)));
    barrier.back().setPosition(2211.84,3737.6);
    barrier.push_back(RectangleShape(Vector2f(133.12,163.84)));
    barrier.back().setPosition(2293.76,3840);
    barrier.push_back(RectangleShape(Vector2f(133.12,286.72)));
    barrier.back().setPosition(2334.72,4003.84);
    barrier.push_back(RectangleShape(Vector2f(163.84,122.88)));
    barrier.back().setPosition(2273.28,4280.32);
    barrier.push_back(RectangleShape(Vector2f(133.12,163.84)));
    barrier.back().setPosition(2232.32,4362.24);
    barrier.push_back(RectangleShape(Vector2f(174.08,317.44)));
    barrier.back().setPosition(2160.64,4495.36);
    barrier.push_back(RectangleShape(Vector2f(245.76,215.04)));
    barrier.back().setPosition(2324.48,4679.68);
    barrier.push_back(RectangleShape(Vector2f(317.44,204.8)));
    barrier.back().setPosition(2539.52,4741.12);
    barrier.push_back(RectangleShape(Vector2f(245.76,215.04)));
    barrier.back().setPosition(2846.72,4812.8);
    barrier.push_back(RectangleShape(Vector2f(276.48,204.8)));
    barrier.back().setPosition(3092.48,4894.72);
    barrier.push_back(RectangleShape(Vector2f(327.68,194.56)));
    barrier.back().setPosition(3358.72,4986.88);
    barrier.push_back(RectangleShape(Vector2f(225.28,256)));
    barrier.back().setPosition(3676.16,5089.28);
    barrier.push_back(RectangleShape(Vector2f(122.88,184.32)));
    barrier.back().setPosition(3645.44,5294.08);
    barrier.push_back(RectangleShape(Vector2f(143.36,215.04)));
    barrier.back().setPosition(3502.08,5324.8);
    barrier.push_back(RectangleShape(Vector2f(143.36,30.72)));
    barrier.back().setPosition(1945.6,3041.28);
    barrier.push_back(RectangleShape(Vector2f(102.4,51.2)));
    barrier.back().setPosition(1935.36,3194.88);
    barrier.push_back(RectangleShape(Vector2f(102.4,61.44)));
    barrier.back().setPosition(1884.16,3358.72);
    barrier.push_back(RectangleShape(Vector2f(153.6,61.44)));
    barrier.back().setPosition(4331.52,5416.96);
    barrier.push_back(RectangleShape(Vector2f(573.44,174.08)));
    barrier.back().setPosition(3788.8,7720.96);

    // In case if character clips out of bounds, press G to reset character to last "checkpoint"
    int OOBCheck;
    RectangleShape area1OOB(Vector2f(1546.24,1331.2));
    area1OOB.setPosition(3266.56,6379.52);
    RectangleShape area2OOB(Vector2f(1269.76,1167.36));
    area2OOB.setPosition(1720.32,1658.88);
    RectangleShape area3OOB(Vector2f(1464.32,1259.52));
    area3OOB.setPosition(6123.52,1853.44);

    // [ Parse dialogue file ]
    progressLoading(app, text_loadingText, "[ Loading Text Files ]", loadWheel);
    string text, event;
    ifstream english;
    english.open("Assets\\dialogue_english.txt");
    english.ignore(1000, '\n');
    while (english >> event >> text) {
        string fixedText = "";
        for (int i = 0; i != text.length(); i++) { // Underscore removal
            if (text[i] == '_') fixedText += ' ';
            else fixedText += text[i];
        }
        map_dialogue[event] = fixedText;
    }
    english.close();

    // [ Check if save exists ]
    progressLoading(app, text_loadingText, "[ Checking Save File ]", loadWheel);
    ifstream savefile;
    savefile.open("Assets\\save.txt");
    if (savefile.is_open()) {
        saveExists = true;
        introCutscenePlayed = true;
    }
    savefile.close();

    while (app.isOpen())
    {
        Event event; // Events (for controller/KBM swap)
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed) app.close();
            if (event.type == Event::LostFocus && isPlaying) isPaused = true;
            if (event.type == Event::JoystickButtonPressed || event.type == Event::JoystickMoved) inputMethod = "ctrl";
            if (event.type == Event::KeyPressed) inputMethod = "kb";
        }

        ctrl_x = Joystick::getAxisPosition(0, Joystick::X); // Controller things
        ctrl_y = Joystick::getAxisPosition(0, Joystick::Y);

        if (isPlaying) {
            if (!isPaused) { // General movement for player
                int spritePos = ((frameTick.getElapsedTime().asMilliseconds()/6 % 3) * 26) + playerSpriteOffset;
                if (!isInDialogue && !isinInventory && !isinCombat && !isFleeingCombat && !playerDefeated && !enemyDefeated) {
                    if (firstIntroBlur && introCutscenePlayed) {
                        if (ctrl_y < -50 || Keyboard::isKeyPressed(Keyboard::W)) {
                            spr_player.move(0, -moveIncrement);
                            mainView.move(0, -moveIncrement);
                            spr_player.setTextureRect(IntRect(spritePos, 108, 26, 36));
                            if (footstepTick.getElapsedTime().asSeconds() > 0.5) {
                                footstepCounter++;
                                footstepTick.restart();
                            }
                        }
                        if (ctrl_x < -50 || Keyboard::isKeyPressed(Keyboard::A)) {
                            spr_player.move(-moveIncrement, 0);
                            mainView.move(-moveIncrement, 0);
                            spr_player.setTextureRect(IntRect(spritePos, 36, 26, 36));
                            if (footstepTick.getElapsedTime().asSeconds() > 0.5) {
                                footstepCounter++;
                                footstepTick.restart();
                            }
                        }
                        if (ctrl_y > 50 || Keyboard::isKeyPressed(Keyboard::S)) {
                            spr_player.move(0, moveIncrement);
                            mainView.move(0, moveIncrement);
                            spr_player.setTextureRect(IntRect(spritePos, 0, 26, 36));
                            if (footstepTick.getElapsedTime().asSeconds() > 0.5) {
                                footstepCounter++;
                                footstepTick.restart();
                            }
                        }
                        if (ctrl_x > 50 || Keyboard::isKeyPressed(Keyboard::D)) {
                            spr_player.move(moveIncrement, 0);
                            mainView.move(moveIncrement, 0);
                            spr_player.setTextureRect(IntRect(spritePos, 72, 26, 36));
                            if (footstepTick.getElapsedTime().asSeconds() > 0.5) {
                                footstepCounter++;
                                footstepTick.restart();
                            }
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Equal)) { // OOB reset test?? shits broken for some reason omegalul
                            if (OOBCheck == 1) {
                                spr_player.setPosition(Vector2f(3937, 6429));
                                mainView.setCenter(Vector2f(3963, 6465));
                            }
                            else if (OOBCheck == 2) {
                                spr_player.setPosition(Vector2f(6748, 2028));
                                mainView.setCenter(Vector2f(6774, 2064));
                            }
                            else if (OOBCheck == 3) {
                                spr_player.setPosition(Vector2f(2209, 2262));
                                mainView.setCenter(Vector2f(2235, 2298));
                            }
                        }

                        if (introCutscenePlayed) for (int i = 0; i != barrier.size(); i++) barrierCollision(spr_player, barrier[i]); // Barrier collision
                        if (spr_player.getGlobalBounds().intersects(area1OOB.getGlobalBounds())) OOBCheck = 1; // in case of out of bounds
                        else if (spr_player.getGlobalBounds().intersects(area2OOB.getGlobalBounds())) OOBCheck = 2;
                        else if (spr_player.getGlobalBounds().intersects(area3OOB.getGlobalBounds())) OOBCheck = 3;
                    }

                    if (!worldMusicPlaying) { // play world music if not playing
                        world1Music.play();
                        worldMusicPlaying = true;
                    }

                    if (!firstIntroBlur) { // when you start game, the black screen will fade away
                        if (uiBlur.getFillColor().a > 0) uiBlur.setFillColor(Color(0, 0, 0, uiBlur.getFillColor().a - 1));
                        else firstIntroBlur = true;
                    }

                    if (!introCutscenePlayed) { // intro cutscene where you're walking into the town
                        int spritePos = ((frameTick.getElapsedTime().asMilliseconds()/100 % 3) * 26) + playerSpriteOffset;
                        spr_player.setTextureRect(IntRect(spritePos, 108, 26, 36));
                        if (spr_player.getPosition().y != 7533) spr_player.move(0, -1);
                        else introCutscenePlayed = true;
                    }

                }

                if (!spr_player.getGlobalBounds().intersects(area1OOB.getGlobalBounds()) // ****add more enemy types!!
                    && !spr_player.getGlobalBounds().intersects(area2OOB.getGlobalBounds())
                    && !spr_player.getGlobalBounds().intersects(area3OOB.getGlobalBounds())) {
                    if (footstepCounter > 15 && rand() % 10 > 8 && !isinCombat) { // Enemy encounter
                        world1Music.stop();
                        int enemyType = rand() % 4;
                        isinCombat = true;
                        posBeforeEncounter = spr_player.getPosition();
                        viewBeforeEncounter = mainView.getCenter();
                        enemy.spr_npc.setPosition(Vector2f(spr_player.getPosition().x + 600, spr_player.getPosition().y));
                        enemyHP = 100;
                        if (enemyType == 0) { // Set to random enemy type
                            enemy.spr_npc.setTexture(raptor1);
                            enemy.npc_name = "Kronker";
                        }
                        else if (enemyType == 1) {
                            enemy.spr_npc.setTexture(raptor2);
                            enemy.npc_name = "Maxx";
                        }
                        else if (enemyType == 2) {
                            enemy.spr_npc.setTexture(bird1);
                            enemy.npc_name = "Shaun";
                        }
                        else if (enemyType == 3) {
                            enemy.spr_npc.setTexture(wolf2);
                            enemy.npc_name = "Noer";
                        }
                        alert.play();
                    }
                }
                else footstepCounter = 0;
            }

            if (isinCombat && footstepTick.getElapsedTime().asSeconds() > 1 && !isFleeingCombat && !isCombatPlayerAttacking && !enemyDefeated && !playerDefeated) { // Initiate Combat
                if (spr_player.getPosition().x > posBeforeEncounter.x - 300) spr_player.move(-5, 0);
                if (enemy.spr_npc.getPosition().x > posBeforeEncounter.x + 300) enemy.spr_npc.move(-5, 0);
                enemy.spr_npc.setColor(Color(255, 255, 255, 255));
                spr_player.setTextureRect(IntRect(26 + playerSpriteOffset, 72, 26, 36));
                if (!musicAlreadyPlaying) {
                    fightMusic.play();
                    musicAlreadyPlaying = true;
                }
            }

            if (isFleeingCombat) { // Player fleeing combat
                if (footstepTick.getElapsedTime().asSeconds() < 2) {
                    if (fightMusic.getVolume() > 1) fightMusic.setVolume(fightMusic.getVolume() - 1);
                    if (enemy.spr_npc.getPosition().x < posBeforeEncounter.x + 800) enemy.spr_npc.move(5, 0);
                    if (!enemyDefeated) {
                        text_combatHint.setString("I don't have time for this.");
                    }
                }
                else { // end fight etc
                    if (spr_player.getPosition().x < posBeforeEncounter.x && !playerDefeated) spr_player.move(5, 0);
                    else {
                        fightMusic.stop();
                        if (!playerDefeated) { // Player is dead
                            spr_player.setPosition(posBeforeEncounter);
                            mainView.setCenter(viewBeforeEncounter);
                        }
                        else {
                            if (OOBCheck == 1) { // Reset player to medic camp thingies
                                spr_player.setPosition(Vector2f(4513, 7350));
                                mainView.setCenter(Vector2f(4539, 7386));
                            }
                            else if (OOBCheck == 2) {
                                spr_player.setPosition(Vector2f(1831, 2409));
                                mainView.setCenter(Vector2f(1857, 2445));
                            }
                            else if (OOBCheck == 3) {
                                spr_player.setPosition(Vector2f(6409, 2058));
                                mainView.setCenter(Vector2f(6435, 2094));
                            }
                        }
                        uiInventorySelection = 1;
                        fightMusic.setVolume(100);
                        isFleeingCombat = false;
                        musicAlreadyPlaying = false;
                        isinCombat = false;
                        isCombatSelection = false;
                        globalSoundPlayed = false;
                        enemyDefeated = false;
                        playerDefeated = false;
                        worldMusicPlaying = false;
                        footstepCounter = 0;
                        firstIntroBlur = false;
                    }
                }
            }

            if (isCombatPlayerAttacking) { // Player is attacking
                if (uiInventorySelection == 1) text_combatDamageAmount.setString("-20HP");
                else if (uiInventorySelection == 2) text_combatDamageAmount.setString("-30HP");
                else if (uiInventorySelection == 3) text_combatDamageAmount.setString("-40HP");
                combatEffect.setPosition(Vector2f(enemy.spr_npc.getPosition().x - 50, enemy.spr_npc.getPosition().y));
                text_combatDamageAmount.setPosition(Vector2f(enemy.spr_npc.getPosition().x + 40, text_combatDamageAmount.getPosition().y - 1));
                if (text_combatDamageAmount.getFillColor().a > 5) text_combatDamageAmount.setFillColor(Color(255, 0, 0, text_combatDamageAmount.getFillColor().a - 5));
                if (uiTick.getElapsedTime().asSeconds() < 1) {
                    combatEffect.setFillColor(Color(255, 255, 255, 255));
                    if (PlayerFile["player_weapon_equipped"] == 5) { // Sword
                        if (uiInventorySelection == 1) {
                            combatEffect.setTexture(&sword1tex);
                            if (!globalSoundPlayed) sword1.play();
                        }
                        else if (uiInventorySelection == 2) {
                            combatEffect.setTexture(&sword2tex);
                            if (!globalSoundPlayed) sword2.play();
                        }
                        else if (uiInventorySelection == 3) {
                            combatEffect.setTexture(&sword3tex);
                            if (!globalSoundPlayed) sword3.play();
                        }
                    }
                    else if (PlayerFile["player_weapon_equipped"] == 6) { // Eyelander
                        if (uiInventorySelection == 1) {
                            combatEffect.setTexture(&eyelander1tex);
                            if (!globalSoundPlayed) eyelander1.play();
                        }
                        else if (uiInventorySelection == 2) {
                            combatEffect.setTexture(&eyelander2tex);
                            if (!globalSoundPlayed) eyelander2.play();
                        }
                        else if (uiInventorySelection == 3) {
                            combatEffect.setTexture(&eyelander3tex);
                            if (!globalSoundPlayed) eyelander3.play();
                        }
                    }
                    else if (PlayerFile["player_weapon_equipped"] == 7) { // Spear
                        if (uiInventorySelection == 1) {
                            combatEffect.setTexture(&spear1tex);
                            if (!globalSoundPlayed) spear1.play();
                        }
                        else if (uiInventorySelection == 2) {
                            combatEffect.setTexture(&spear2tex);
                            if (!globalSoundPlayed) spear2.play();
                        }
                        else if (uiInventorySelection == 3) {
                            combatEffect.setTexture(&spear3tex);
                            if (!globalSoundPlayed) spear3.play();
                        }
                    }
                    else if (PlayerFile["player_weapon_equipped"] == 8) { // Magic Staff
                        if (uiInventorySelection == 1) {
                            combatEffect.setTexture(&staff1tex);
                            if (!globalSoundPlayed) staff1.play();
                        }
                        else if (uiInventorySelection == 2) {
                            combatEffect.setTexture(&staff2tex);
                            if (!globalSoundPlayed) staff2.play();
                        }
                        else if (uiInventorySelection == 3) {
                            combatEffect.setTexture(&staff3tex);
                            if (!globalSoundPlayed) staff3.play();
                        }
                    }
                    else if (PlayerFile["player_weapon_equipped"] == 9) { // Revolver
                        if (uiInventorySelection == 1) {
                            combatEffect.setTexture(&revolver1tex);
                            if (!globalSoundPlayed) revolver1.play();
                        }
                        else if (uiInventorySelection == 2) {
                            combatEffect.setTexture(&revolver2tex);
                            if (!globalSoundPlayed) revolver2.play();
                        }
                        else if (uiInventorySelection == 3) {
                            combatEffect.setTexture(&revolver3tex);
                            if (!globalSoundPlayed) revolver3.play();
                        }
                    }
                    else if (PlayerFile["player_weapon_equipped"] == 10) { // Crossbow
                        if (uiInventorySelection == 1) {
                            combatEffect.setTexture(&crossbow1tex);
                            if (!globalSoundPlayed) crossbow1.play();
                        }
                        else if (uiInventorySelection == 2) {
                            combatEffect.setTexture(&crossbow2tex);
                            if (!globalSoundPlayed) crossbow2.play();
                        }
                        else if (uiInventorySelection == 3) {
                            combatEffect.setTexture(&crossbow3tex);
                            if (!globalSoundPlayed) crossbow3.play();
                        }
                    }
                    globalSoundPlayed = true;
                    if (spr_player.getPosition().x < posBeforeEncounter.x - 100) spr_player.move(5, 0); // revert back to pos
                }
                else if (uiTick.getElapsedTime().asSeconds() > 1 && uiTick.getElapsedTime().asSeconds() < 2) { // run up to enemy
                    if (combatEffect.getFillColor().a > 10) combatEffect.setFillColor(Color(255, 255, 255, combatEffect.getFillColor().a - 10));
                    if (spr_player.getPosition().x > posBeforeEncounter.x - 300) spr_player.move(-5, 0);
                }
                else {
                    if (uiInventorySelection == 1) enemyHP -= 20;
                    else if (uiInventorySelection == 2) enemyHP -= 30;
                    else if (uiInventorySelection == 3) enemyHP -= 40;
                    combatEffect.setFillColor(Color(255, 255, 255, 255));
                    text_combatDamageAmount.setFillColor(Color(255, 0, 0, 255));
                    if (enemyHP <= 0) enemyDefeated = true;
                    else {
                        isCombatEnemyAttacking = true;
                        globalSoundPlayed = false;
                        combatEnemyNextMove = rand() % 3;
                        text_combatDamageAmount.setPosition(Vector2f(spr_player.getPosition().x - 50, spr_player.getPosition().y - 50));
                    }
                    isCombatPlayerAttacking = false;
                    uiTick.restart();
                }
            }

            if (playerHealed) { // Heal sequence
                if (uiInventorySelection == 1) text_combatDamageAmount.setString("+20HP");
                else if (uiInventorySelection == 2) text_combatDamageAmount.setString("+50HP");
                else if (uiInventorySelection == 3) text_combatDamageAmount.setString("+30HP");
                else if (uiInventorySelection == 4) text_combatDamageAmount.setString("+15HP");
                else if (uiInventorySelection == 5) text_combatDamageAmount.setString("+100HP");
                text_combatDamageAmount.setPosition(Vector2f(spr_player.getPosition().x - 50, text_combatDamageAmount.getPosition().y - 1));
                combatEffect.setPosition(Vector2f(spr_player.getPosition().x - 100, spr_player.getPosition().y));
                if (text_combatDamageAmount.getFillColor().a > 5) text_combatDamageAmount.setFillColor(Color(0, 255, 0, text_combatDamageAmount.getFillColor().a - 5));
                if (combatEffect.getFillColor().a > 10) combatEffect.setFillColor(Color(255, 255, 255, combatEffect.getFillColor().a - 10));
                combatEffect.setTexture(&healtex);

                if (!globalSoundPlayed) {
                    heal.play();
                    globalSoundPlayed = true;
                }

                if (uiTick.getElapsedTime().asSeconds() > 2) {
                    playerHealed = false;
                    globalSoundPlayed = false;
                    combatEnemyNextMove = rand() % 3;
                    text_combatDamageAmount.setFillColor(Color(255, 0, 0, 255));
                    text_combatDamageAmount.setPosition(Vector2f(spr_player.getPosition().x - 50, spr_player.getPosition().y - 50));
                    isCombatEnemyAttacking = true;
                    uiTick.restart();
                }
            }

            if (isCombatEnemyAttacking) { // Enemy is attacking
                if (combatEnemyNextMove == 0) text_combatDamageAmount.setString("-10HP");
                else if (combatEnemyNextMove == 1) text_combatDamageAmount.setString("-20HP");
                else if (combatEnemyNextMove == 2) text_combatDamageAmount.setString("-30HP");
                combatEffect.setPosition(Vector2f(spr_player.getPosition().x - 100, spr_player.getPosition().y));
                text_combatDamageAmount.setPosition(Vector2f(spr_player.getPosition().x - 50, text_combatDamageAmount.getPosition().y - 1));
                if (text_combatDamageAmount.getFillColor().a > 5) text_combatDamageAmount.setFillColor(Color(255, 0, 0, text_combatDamageAmount.getFillColor().a - 5));
                if (uiTick.getElapsedTime().asSeconds() < 1) {
                    combatEffect.setFillColor(Color(255, 255, 255, 255));
                    if (combatEnemyNextMove == 0 || combatEnemyNextMove == 1 || combatEnemyNextMove == 2) {
                        combatEffect.setTexture(&sword1tex);
                        if (!globalSoundPlayed) {
                            sword1.play();
                            globalSoundPlayed = true;
                        }
                    }
                    if (enemy.spr_npc.getPosition().x > posBeforeEncounter.x + 100) enemy.spr_npc.move(-5, 0);
                }
                else if (uiTick.getElapsedTime().asSeconds() > 1 && uiTick.getElapsedTime().asSeconds() < 2) {
                    if (combatEffect.getFillColor().a > 10) combatEffect.setFillColor(Color(255, 255, 255, combatEffect.getFillColor().a - 10));
                    if (enemy.spr_npc.getPosition().x < posBeforeEncounter.x + 300) enemy.spr_npc.move(5, 0);
                }
                else {
                    if (combatEnemyNextMove == 0) PlayerFile["inv_health"] -= 10;
                    else if (combatEnemyNextMove == 1) PlayerFile["inv_health"] -= 20;
                    else if (combatEnemyNextMove == 2) PlayerFile["inv_health"] -= 30;
                    combatEffect.setFillColor(Color(255, 255, 255, 255));
                    text_combatDamageAmount.setFillColor(Color(255, 0, 0, 255));
                    uiInventorySelection = 1;
                    if (PlayerFile["inv_health"] <= 0 && !playerDefeated) {
                        playerDefeated = true;
                        uiBlur.setFillColor(Color(0, 0, 0, 0));
                    }
                    else {
                        isCombatSelection = false;
                        globalSoundPlayed = false;
                    }
                    isCombatEnemyAttacking = false;
                    uiTick.restart();
                }
            }

            // ui elements
            if (uiTick.getElapsedTime().asSeconds() > 0.25) { // note to self: don't put ui elements into a single event like this next time
                text_helpGold.setString(to_string(PlayerFile["inv_gold"]));
                text_helpHealth.setString(to_string(PlayerFile["inv_health"]));
                for (int i = 0; i != spr_npcs.size(); i++) { // Dialogue Box Collision
                    if (npcCollision(spr_player, spr_npcs[i].spr_npc) && !isInDialogue && !isinInventory && !isinCombat && spr_npcs[i].npc_type != "enemy") {
                        currentNPC = spr_npcs[i].npc_type;
                        spr_npcs[i].text_dialoguetrigger.setFillColor(Color(255, 0, 220, 255));
                        spr_npcs[i].text_dialoguetrigger.setOutlineThickness(1);
                        if (Keyboard::isKeyPressed(Keyboard::F) || Joystick::isButtonPressed(0, 0)) {
                            viewBeforeCutscene = mainView.getCenter();
                            dialogue.play();
                            currentDialogue = 0;
                            ui_dialogueSpeaker.setString(spr_npcs[i].npc_name);
                            isInDialogue = true;
                            uiTick.restart();
                        }
                    }
                    else if (!isInDialogue) {
                        currentNPC = "";
                        spr_npcs[i].text_dialoguetrigger.setFillColor(Color(255, 0, 220, 0));
                        spr_npcs[i].text_dialoguetrigger.setOutlineThickness(0);
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Escape) || Joystick::isButtonPressed(0, 7)) {
                    uiMenuSelection = 1;
                    if (isPaused) isPaused = false;
                    else isPaused = true;
                    uiBlur.setFillColor(Color(0, 0, 0, 0));
                    uiTick.restart();
                }

                if (!isPaused) {
                    if (enemyDefeated) { // rip enemy lol
                        if (fightMusic.getVolume() > 1) fightMusic.setVolume(fightMusic.getVolume() - 1);
                        if (enemy.spr_npc.getColor().a > 5) enemy.spr_npc.setColor(Color(255, 255, 255, enemy.spr_npc.getColor().a - 5));
                        ui_combatText.setString("Victory! You defeated " + enemy.npc_name + "!\nYou gained 30 Gold.");
                        if (enemyHP < 0) enemyHP = 0;
                        if (uiTick.getElapsedTime().asSeconds() > 4 && !isFleeingCombat) {
                            PlayerFile["inv_gold"] += 30;
                            isFleeingCombat = true;
                            uiTick.restart();
                        }
                    }
                    if (playerDefeated) {
                        if (fightMusic.getVolume() > 1) fightMusic.setVolume(fightMusic.getVolume() - 1);
                        if (enemy.spr_npc.getPosition().x < posBeforeEncounter.x + 800) enemy.spr_npc.move(5, 0);
                        if (uiBlur.getFillColor().a < 255) uiBlur.setFillColor(Color(0, 0, 0, uiBlur.getFillColor().a + 3));
                        ui_combatText.setString("I don't feel so good.\nYou lost 50 Gold.");
                        if (uiTick.getElapsedTime().asSeconds() > 6 && !isFleeingCombat) {
                            PlayerFile["inv_gold"] -= 50;
                            if (PlayerFile["inv_gold"] < 0) PlayerFile["inv_gold"] = 0;
                            PlayerFile["inv_health"] = 100;
                            isFleeingCombat = true;
                            uiTick.restart();
                        }
                    }
                    if (isinCombat && footstepTick.getElapsedTime().asSeconds() > 1 && !isFleeingCombat && !enemyDefeated && !playerDefeated && !playerHealed) { // In Combat
                        if (isCombatInventory) { // Combat inventory
                            if (Keyboard::isKeyPressed(Keyboard::Tab) || Joystick::isButtonPressed(0, 1)) {
                                select.play();
                                isCombatInventory = false;
                                uiInventorySelection = 1;
                                uiTick.restart();
                            }

                            if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                                dialogue.play();
                                uiInventorySelection++;
                                uiTick.restart();
                            }
                            else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                                dialogue.play();
                                uiInventorySelection--;
                                uiTick.restart();
                            }

                            if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                                select.play();
                                if (PlayerFile["inv_health"] < 100) {
                                    if (uiInventorySelection == 1) { // Apple
                                        if (PlayerFile["inv_apple_amt"] > 0) {
                                            PlayerFile["inv_apple_amt"]--;
                                            PlayerFile["inv_health"] += 20;
                                        }
                                    }
                                    else if (uiInventorySelection == 2) {
                                        if (PlayerFile["inv_spcapple_amt"] > 0) {
                                            PlayerFile["inv_spcapple_amt"]--;
                                            PlayerFile["inv_health"] += 50;
                                        }
                                    }
                                    else if (uiInventorySelection == 3) {
                                        if (PlayerFile["inv_meat_amt"] > 0) {
                                            PlayerFile["inv_meat_amt"]--;
                                            PlayerFile["inv_health"] += 30;
                                        }
                                    }
                                    else if (uiInventorySelection == 4) {
                                        if (PlayerFile["inv_herb_amt"] > 0) {
                                            PlayerFile["inv_herb_amt"]--;
                                            PlayerFile["inv_health"] += 15;
                                        }
                                    }
                                    else if (uiInventorySelection == 5) {
                                        if (PlayerFile["inv_hpotion_amt"] > 0) {
                                            PlayerFile["inv_hpotion_amt"]--;
                                            PlayerFile["inv_health"] += 100;
                                        }
                                    }
                                    combatEnemyNextMove = rand() % 3;
                                    text_combatDamageAmount.setPosition(Vector2f(spr_player.getPosition().x - 50, spr_player.getPosition().y - 50));
                                    combatEffect.setFillColor(Color(255, 255, 255, 255));
                                    playerHealed = true;
                                    isCombatInventory = false;
                                    isCombatSelection = false;
                                }

                                if (PlayerFile["inv_health"] > 100) PlayerFile["inv_health"] = 100;

                                uiTick.restart();
                            }

                            if (uiInventorySelection < 1) uiInventorySelection = 5;
                            if (uiInventorySelection > 5) uiInventorySelection = 1;

                            for (int i = 0; i != combatInventory.size(); i++) {
                                string itemName = combatInventory[i].itemName;
                                if (uiInventorySelection == 1) {
                                    text_combatInventoryHint.setString("An apple a day keeps the monsters away. (+20HP)");
                                    if (itemName == "Apple") combatInventory[i].Container.setFillColor(Color(0, 162, 232));
                                    else combatInventory[i].Container.setFillColor(Color(35, 35, 35));
                                }
                                if (uiInventorySelection == 2) {
                                    text_combatInventoryHint.setString("Golden, just like you! (+50HP)");
                                    if (itemName == "Special Apple") combatInventory[i].Container.setFillColor(Color(0, 162, 232));
                                    else combatInventory[i].Container.setFillColor(Color(35, 35, 35));
                                }
                                if (uiInventorySelection == 3) {
                                    text_combatInventoryHint.setString("Herbivores hate them! Learn the secret today! (+30HP)");
                                    if (itemName == "Meat") combatInventory[i].Container.setFillColor(Color(0, 162, 232));
                                    else combatInventory[i].Container.setFillColor(Color(35, 35, 35));
                                }
                                if (uiInventorySelection == 4) {
                                    text_combatInventoryHint.setString("It's just a fancy name for grass. (+15HP)");
                                    if (itemName == "Herb") combatInventory[i].Container.setFillColor(Color(0, 162, 232));
                                    else combatInventory[i].Container.setFillColor(Color(35, 35, 35));
                                }
                                if (uiInventorySelection == 5) {
                                    text_combatInventoryHint.setString("Fills you with determination. (+100HP)");
                                    if (itemName == "Health Potion") combatInventory[i].Container.setFillColor(Color(0, 162, 232));
                                    else combatInventory[i].Container.setFillColor(Color(35, 35, 35));
                                }
                            }
                        }

                        if (isCombatSelection && !isCombatPlayerAttacking && !isCombatEnemyAttacking && !playerHealed) { // Combat selection
                            if (Keyboard::isKeyPressed(Keyboard::Tab) || Joystick::isButtonPressed(0, 1)) {
                                select.play();
                                isCombatSelection = false;
                                uiInventorySelection = 1;
                                uiTick.restart();
                            }

                            if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                                dialogue.play();
                                uiInventorySelection++;
                                uiTick.restart();
                            }
                            else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                                dialogue.play();
                                uiInventorySelection--;
                                uiTick.restart();
                            }

                            if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 1)) {
                                select.play();
                                text_combatDamageAmount.setPosition(Vector2f(enemy.spr_npc.getPosition().x + 40, enemy.spr_npc.getPosition().y - 50));
                                isCombatPlayerAttacking = true;
                                uiTick.restart();
                            }

                            if (uiInventorySelection < 1) uiInventorySelection = 3;
                            if (uiInventorySelection > 3) uiInventorySelection = 1;

                            button_combatFightMove1.setFillColor(Color(35, 35, 35));
                            button_combatFightMove2.setFillColor(Color(35, 35, 35));
                            button_combatFightMove3.setFillColor(Color(35, 35, 35));

                            if (uiInventorySelection == 1) button_combatFightMove1.setFillColor(Color(0, 162, 232));
                            else if (uiInventorySelection == 2) button_combatFightMove2.setFillColor(Color(0, 162, 232));
                            else if (uiInventorySelection == 3) button_combatFightMove3.setFillColor(Color(0, 162, 232));

                            if (PlayerFile["player_weapon_equipped"] == 5) { // Adjust weapon moveset text
                                text_combatWeapon.setString("Weapon: Sword");
                                text_combatFightMove1.setString("(20DMG) Flick");
                                text_combatFightMove2.setString("(30DMG) Slash");
                                text_combatFightMove3.setString("(40DMG) Lunge");
                            }
                            else if (PlayerFile["player_weapon_equipped"] == 6) {
                                text_combatWeapon.setString("Weapon: Eyelander");
                                text_combatFightMove1.setString("(20DMG) Charge");
                                text_combatFightMove2.setString("(30DMG) Scottish Slash");
                                text_combatFightMove3.setString("(40DMG) Headtaker");
                            }
                            else if (PlayerFile["player_weapon_equipped"] == 7) {
                                text_combatWeapon.setString("Weapon: Spear");
                                text_combatFightMove1.setString("(20DMG) Poke It With Stick");
                                text_combatFightMove2.setString("(30DMG) Impale");
                                text_combatFightMove3.setString("(40DMG) Hunting Season");
                            }
                            else if (PlayerFile["player_weapon_equipped"] == 8) {
                                text_combatWeapon.setString("Weapon: Magic Staff");
                                text_combatFightMove1.setString("(20DMG) Next Trick");
                                text_combatFightMove2.setString("(30DMG) Affiction");
                                text_combatFightMove3.setString("(40DMG) Nova");
                            }
                            else if (PlayerFile["player_weapon_equipped"] == 9) {
                                text_combatWeapon.setString("Weapon: Revolver");
                                text_combatFightMove1.setString("(20DMG) Steady Shot");
                                text_combatFightMove2.setString("(30DMG) Hammer");
                                text_combatFightMove3.setString("(40DMG) Deadeye");
                            }
                            else if (PlayerFile["player_weapon_equipped"] == 10) {
                                text_combatWeapon.setString("Weapon: Crossbow");
                                text_combatFightMove1.setString("(20DMG) Shoot");
                                text_combatFightMove2.setString("(30DMG) Silent Shot");
                                text_combatFightMove3.setString("(40DMG) Poison Dart");
                            }
                        }

                        if (!isCombatInventory && !isCombatSelection && !playerHealed && !isCombatEnemyAttacking && !isCombatPlayerAttacking) {
                            if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                                dialogue.play();
                                uiInventorySelection++;
                                uiTick.restart();
                            }
                            else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                                dialogue.play();
                                uiInventorySelection--;
                                uiTick.restart();
                            }

                            if (uiInventorySelection < 1) uiInventorySelection = 3;
                            if (uiInventorySelection > 3) uiInventorySelection = 1;

                            if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                                select.play();
                                if (uiInventorySelection == 1) {
                                    isCombatSelection = true;
                                    uiInventorySelection = 1;
                                    uiTick.restart();
                                }
                                else if (uiInventorySelection == 2) {
                                    isCombatInventory = true;
                                    uiInventorySelection = 1;
                                    uiTick.restart();
                                }
                                else if (uiInventorySelection == 3) {
                                    isFleeingCombat = true;
                                    uiInventorySelection = 1;
                                    footstepTick.restart();
                                    uiTick.restart();
                                }

                                uiTick.restart();
                            }
                        }

                        if (winterBack.getFillColor().a < 250) winterBack.setFillColor(Color(255, 255, 255, winterBack.getFillColor().a + 10));

                        button_combatSelection1.setFillColor(Color(35, 35, 35));
                        button_combatSelection2.setFillColor(Color(35, 35, 35));
                        button_combatSelection3.setFillColor(Color(35, 35, 35));

                        if (uiInventorySelection == 1) {
                            text_combatHint.setString("En garde!");
                            button_combatSelection1.setFillColor(Color(0, 162, 232));
                        }
                        else if (uiInventorySelection == 2) {
                            text_combatHint.setString("Top up on your health.");
                            button_combatSelection2.setFillColor(Color(0, 162, 232));
                        }
                        else if (uiInventorySelection == 3) {
                            text_combatHint.setString("Fleeing can only get you so far.");
                            button_combatSelection3.setFillColor(Color(0, 162, 232));
                        }

                        for (int i = 0; i != combatInventory.size(); i++) { // Update inventory
                            string itemName = combatInventory[i].itemName;
                            if (itemName == "Apple") combatInventory[i].Amount.setString(to_string(PlayerFile["inv_apple_amt"]));
                            if (itemName == "Special Apple") combatInventory[i].Amount.setString(to_string(PlayerFile["inv_spcapple_amt"]));
                            if (itemName == "Meat") combatInventory[i].Amount.setString(to_string(PlayerFile["inv_meat_amt"]));
                            if (itemName == "Herb") combatInventory[i].Amount.setString(to_string(PlayerFile["inv_herb_amt"]));
                            if (itemName == "Health Potion") combatInventory[i].Amount.setString(to_string(PlayerFile["inv_hpotion_amt"]));
                        }
                    }
                    else if (isFleeingCombat && footstepTick.getElapsedTime().asSeconds() > 2) { // Fade background
                        if (winterBack.getFillColor().a > 0) winterBack.setFillColor(Color(255, 255, 255, winterBack.getFillColor().a - 10));
                    }
                    ui_combatPlayerHealth.Amount.setString(to_string(PlayerFile["inv_health"]));
                    ui_combatEnemyHealth.Amount.setString(to_string(enemyHP));
                    text_combatEnemyTag.setString(enemy.npc_name);
                }

                if (isinShop && !isPaused) { // Exit shop
                    if (Keyboard::isKeyPressed(Keyboard::Tab) || Joystick::isButtonPressed(0, 2)) {
                        isinShop = false;
                        uiInventorySelection = 1;
                        currentDialogue = 3;
                        uiTick.restart();
                    }

                    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                        dialogue.play();
                        uiInventorySelection++;
                        uiTick.restart();
                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                        dialogue.play();
                        uiInventorySelection--;
                        uiTick.restart();
                    }

                    if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) { // Use item
                        if (uiInventorySelection == 1) {
                                if (PlayerFile["inv_gold"] >= 50) {
                                PlayerFile["inv_apple_amt"]++;
                                PlayerFile["inv_gold"] -= 50;
                                currentDialogue = 2;
                                select.play();
                                uiTick.restart();
                            }
                            else currentDialogue = 1;
                        }
                        else if (uiInventorySelection == 2) {
                            if (PlayerFile["inv_gold"] >= 150) {
                                PlayerFile["inv_spcapple_amt"]++;
                                PlayerFile["inv_gold"] -= 150;
                                currentDialogue = 2;
                                select.play();
                                uiTick.restart();
                            }
                            else currentDialogue = 1;
                        }
                        else if (uiInventorySelection == 3) {
                            if (PlayerFile["inv_gold"] >= 100) {
                                PlayerFile["inv_meat_amt"]++;
                                PlayerFile["inv_gold"] -= 100;
                                currentDialogue = 2;
                                select.play();
                                uiTick.restart();
                            }
                            else currentDialogue = 1;
                        }
                        else if (uiInventorySelection == 4) {
                            if (PlayerFile["inv_gold"] >= 40) {
                                PlayerFile["inv_herb_amt"]++;
                                PlayerFile["inv_gold"] -= 40;
                                currentDialogue = 2;
                                select.play();
                                uiTick.restart();
                            }
                            else currentDialogue = 1;
                        }
                        else if (uiInventorySelection == 5) {
                            if (PlayerFile["inv_gold"] >= 200) {
                                PlayerFile["inv_hpotion_amt"]++;
                                PlayerFile["inv_gold"] -= 200;
                                currentDialogue = 2;
                                select.play();
                                uiTick.restart();
                            }
                            else currentDialogue = 1;
                        }
                    }

                    if (uiInventorySelection < 1) uiInventorySelection = 5;
                    if (uiInventorySelection > 5) uiInventorySelection = 1;
                    for (int i = 0; i != shopInventory.size(); i++) {
                        string itemName = shopInventory[i].itemName;
                        if (uiInventorySelection == 1) {
                            if (itemName == "Apple") shopInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else shopInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 2) {
                            if (itemName == "Special Apple") shopInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else shopInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 3) {
                            if (itemName == "Meat") shopInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else shopInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 4) {
                            if (itemName == "Herb") shopInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else shopInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 5) {
                            if (itemName == "Health Potion") shopInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else shopInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        // Update amounts
                        if (itemName == "Apple") shopInventory[i].Amount.setString("50G - " + to_string(PlayerFile["inv_apple_amt"]));
                        if (itemName == "Special Apple") shopInventory[i].Amount.setString("150G - " + to_string(PlayerFile["inv_spcapple_amt"]));
                        if (itemName == "Meat") shopInventory[i].Amount.setString("100G - " + to_string(PlayerFile["inv_meat_amt"]));
                        if (itemName == "Herb") shopInventory[i].Amount.setString("40G - " + to_string(PlayerFile["inv_herb_amt"]));
                        if (itemName == "Health Potion") shopInventory[i].Amount.setString("200G - " + to_string(PlayerFile["inv_hpotion_amt"]));
                        if (itemName == "Gold") shopInventory[i].Amount.setString(to_string(PlayerFile["inv_gold"]));
                    }
                }

                if (isInDialogue && !isPaused) { // Dialogue progression
                    if (!isinShop) {
                        if (Keyboard::isKeyPressed(Keyboard::Z) || Joystick::isButtonPressed(0, 1)) {
                            dialogue.play();
                            currentDialogue++;
                            uiTick.restart();
                        }
                    }
                    // NPC Dialogue (dialogue_english.txt)
                    // area 1
                    if (currentNPC == "guard1") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["guard1_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["guard1_intro2"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "guard2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["guard2_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["guard2_intro2"]);
                        else if (currentDialogue == 2) ui_dialogueText.setString(map_dialogue["guard2_intro3"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "guard3") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["guard3_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["guard3_intro2"]);
                        else if (currentDialogue == 2) ui_dialogueText.setString(map_dialogue["guard3_intro3"]);
                        else if (currentDialogue == 3) ui_dialogueText.setString(map_dialogue["guard3_intro4"]);
                        else if (currentDialogue == 4) {
                            ui_dialogueText.setString(map_dialogue["guard3_intro5"]);
                            mainView.setCenter(Vector2f(4497, 7326));
                        }
                        else if (currentDialogue == 5) {
                            ui_dialogueText.setString(map_dialogue["guard3_intro6"]);
                            mainView.setCenter(Vector2f(3516, 6981));
                        }
                        else if (currentDialogue == 6) {
                            ui_dialogueText.setString(map_dialogue["guard3_intro7"]);
                            mainView.setCenter(viewBeforeCutscene);
                        }
                        else isInDialogue = false;
                    }

                    if (currentNPC == "shopkeeper1") {
                        if (currentDialogue == 0) {
                            ui_dialogueText.setString(map_dialogue["shopkeeper1_intro1"]);
                            isinShop = true;
                            ui_dialogueHint.setString("...");
                        }
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["shopkeeper1_poor1"]);
                        else if (currentDialogue == 2) ui_dialogueText.setString(map_dialogue["shopkeeper1_success1"]);
                        else if (currentDialogue == 3) {
                            ui_dialogueText.setString(map_dialogue["shopkeeper1_thanks1"]);
                            ui_dialogueHint.setString("Press [ Z ] to continue");
                        }
                        else isInDialogue = false;
                    }

                    if (currentNPC == "healer1") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["healer1_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["healer1_hurt1"]);
                        else if (currentDialogue == 2) {
                            if (!globalSoundPlayed) {
                                heal.play();
                                globalSoundPlayed = true;
                            }
                            PlayerFile["inv_health"] = 100;
                            ui_dialogueText.setString(map_dialogue["healer1_full1"]);
                        }
                        else {
                            globalSoundPlayed = false;
                            isInDialogue = false;
                        }
                    }

                    if (currentNPC == "npc1a1") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc1a1_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc1a1_intro2"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc2a1") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc2a1_intro1"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc3a1" || currentNPC == "npc4a1") {
                        if (currentDialogue == 0) {
                            ui_dialogueSpeaker.setString("Kareem");
                            ui_dialogueText.setString(map_dialogue["npc3a1_intro1"]);
                        }
                        else if (currentDialogue == 1) {
                            ui_dialogueSpeaker.setString("Paul");
                            ui_dialogueText.setString(map_dialogue["npc4a1_intro1"]);
                        }
                        else if (currentDialogue == 2) {
                            ui_dialogueSpeaker.setString("Kareem");
                            ui_dialogueText.setString(map_dialogue["npc3a1_intro2"]);
                        }
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc5a1") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc5a1_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc5a1_intro2"]);
                        else isInDialogue = false;
                    }

                    // area 2
                    if (currentNPC == "shopkeeper2") {
                        if (currentDialogue == 0) {
                            ui_dialogueText.setString(map_dialogue["shopkeeper2_intro1"]);
                            isinShop = true;
                            ui_dialogueHint.setString("...");
                        }
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["shopkeeper2_poor1"]);
                        else if (currentDialogue == 2) ui_dialogueText.setString(map_dialogue["shopkeeper2_success1"]);
                        else if (currentDialogue == 3) {
                            ui_dialogueText.setString(map_dialogue["shopkeeper2_thanks1"]);
                            ui_dialogueHint.setString("Press [ Z ] to continue");
                        }
                        else isInDialogue = false;
                    }

                    if (currentNPC == "healer2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["healer2_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["healer2_hurt1"]);
                        else if (currentDialogue == 2) {
                            if (!globalSoundPlayed) {
                                heal.play();
                                globalSoundPlayed = true;
                            }
                            PlayerFile["inv_health"] = 100;
                            ui_dialogueText.setString(map_dialogue["healer2_full1"]);
                        }
                        else {
                            globalSoundPlayed = false;
                            isInDialogue = false;
                        }
                    }

                    if (currentNPC == "npc1a2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc1a2_intro1"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc2a2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc2a2_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc2a2_intro2"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc3a2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc3a2_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc3a2_intro2"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc4a2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc4a2_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc4a2_intro2"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc5a2") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc5a2_intro1"]);
                        else isInDialogue = false;
                    }

                    // area 3
                    if (currentNPC == "shopkeeper3") {
                        if (currentDialogue == 0) {
                            ui_dialogueText.setString(map_dialogue["shopkeeper3_intro1"]);
                            isinShop = true;
                            ui_dialogueHint.setString("...");
                        }
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["shopkeeper3_poor1"]);
                        else if (currentDialogue == 2) ui_dialogueText.setString(map_dialogue["shopkeeper3_success1"]);
                        else if (currentDialogue == 3) {
                            ui_dialogueText.setString(map_dialogue["shopkeeper3_thanks1"]);
                            ui_dialogueHint.setString("Press [ Z ] to continue");
                        }
                        else isInDialogue = false;
                    }

                    if (currentNPC == "healer3") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["healer3_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["healer3_hurt1"]);
                        else if (currentDialogue == 2) {
                            if (!globalSoundPlayed) {
                                heal.play();
                                globalSoundPlayed = true;
                            }
                            PlayerFile["inv_health"] = 100;
                            ui_dialogueText.setString(map_dialogue["healer3_full1"]);
                        }
                        else {
                            globalSoundPlayed = false;
                            isInDialogue = false;
                        }
                    }

                    if (currentNPC == "npc1a3") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc1a3_intro1"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc2a3") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc2a3_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc2a3_intro2"]);
                        else isInDialogue = false;
                    }

                    if (currentNPC == "npc3a3" || currentNPC == "npc4a3") {
                        if (currentDialogue == 0) ui_dialogueText.setString(map_dialogue["npc3a3_intro1"]);
                        else if (currentDialogue == 1) ui_dialogueText.setString(map_dialogue["npc4a3_intro1"]);
                        else if (currentDialogue == 2) ui_dialogueText.setString(map_dialogue["npc3a3_intro2"]);
                        else isInDialogue = false;
                    }
                }

                if (!isPaused && !isInDialogue && !isinCombat && !isinShop) { // Toggle inventory
                    if (Keyboard::isKeyPressed(Keyboard::Tab) || Joystick::isButtonPressed(0, 3)) {
                        inventory.play();
                        if (isinInventory) {
                            isinInventory = false;
                            uiInventorySelection = 1;
                        }
                        else isinInventory = true;
                        uiTick.restart();
                    }
                }

                if (isinInventory && !isPaused) { // Use inventory
                    if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                        select.play();
                        if (uiInventorySelection == 5 || uiInventorySelection == 6 || uiInventorySelection == 7 || uiInventorySelection == 8 || uiInventorySelection == 9 || uiInventorySelection == 10) {
                            PlayerFile["player_weapon_equipped"] = uiInventorySelection;
                        }

                        if (PlayerFile["inv_health"] < 100) {
                            if (uiInventorySelection == 1) { // Apple
                                if (PlayerFile["inv_apple_amt"] > 0) {
                                    PlayerFile["inv_apple_amt"]--;
                                    PlayerFile["inv_health"] += 20;
                                }
                            }
                            else if (uiInventorySelection == 2) {
                                if (PlayerFile["inv_spcapple_amt"] > 0) {
                                    PlayerFile["inv_spcapple_amt"]--;
                                    PlayerFile["inv_health"] += 50;
                                }
                            }
                            else if (uiInventorySelection == 3) {
                                if (PlayerFile["inv_meat_amt"] > 0) {
                                    PlayerFile["inv_meat_amt"]--;
                                    PlayerFile["inv_health"] += 30;
                                }
                            }
                            else if (uiInventorySelection == 4) {
                                if (PlayerFile["inv_herb_amt"] > 0) {
                                    PlayerFile["inv_herb_amt"]--;
                                    PlayerFile["inv_health"] += 15;
                                }
                            }
                            else if (uiInventorySelection == 11) {
                                if (PlayerFile["inv_hpotion_amt"] > 0) {
                                    PlayerFile["inv_hpotion_amt"]--;
                                    PlayerFile["inv_health"] += 100;
                                }
                            }
                        }

                        if (PlayerFile["inv_health"] > 100) PlayerFile["inv_health"] = 100;

                        uiTick.restart();
                    }

                    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                        dialogue.play();
                        uiInventorySelection++;
                        uiTick.restart();
                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                        dialogue.play();
                        uiInventorySelection--;
                        uiTick.restart();
                    }
                    if (Joystick::isButtonPressed(0, 1)) {
                        pick.play();
                        isinInventory = false;
                        uiInventorySelection = 1;
                        uiTick.restart();
                    }

                    if (uiInventorySelection > 11) uiInventorySelection = 1;
                    if (uiInventorySelection < 1) uiInventorySelection = 11;

                    for (int i = 0; i != uiInventory.size(); i++) { // Update inventory
                        string itemName = uiInventory[i].itemName;
                        if (itemName == "Apple") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_apple_amt"]));
                        if (itemName == "Special Apple") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_spcapple_amt"]));
                        if (itemName == "Meat") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_meat_amt"]));
                        if (itemName == "Herb") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_herb_amt"]));
                        if (itemName == "Health Potion") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_hpotion_amt"]));
                        if (itemName == "Health") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_health"]));
                        if (itemName == "Gold") uiInventory[i].Amount.setString(to_string(PlayerFile["inv_gold"]));

                        if (uiInventorySelection == 1) {
                            ui_inventoryHint.setString("An apple a day keeps the monsters\n away.\n(+20HP)");
                            if (itemName == "Apple") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 2) {
                            ui_inventoryHint.setString("Golden, just like you!\n(+50HP)");
                            if (itemName == "Special Apple") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 3) {
                            ui_inventoryHint.setString("Herbivores hate them! Learn the\nsecret today!\n(+30HP)");
                            if (itemName == "Meat") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 4) {
                            ui_inventoryHint.setString("It's just a fancy name for grass.\n(+15HP)");
                            if (itemName == "Herb") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 5) {
                            ui_inventoryHint.setString("Sharp. Swing towards enemies, not\ntowards friendlies.\n(30DMG)");
                            if (itemName == "Sword") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 6) {
                            ui_inventoryHint.setString("Heads will roll.\n(60DMG)");
                            if (itemName == "Eyelander") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 7) {
                            ui_inventoryHint.setString("Cannibal hunting season.\n(40DMG)");
                            if (itemName == "Spear") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 8) {
                            ui_inventoryHint.setString("You're a wizard!\n(35DMG)");
                            if (itemName == "Magic Staff") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 9) {
                            ui_inventoryHint.setString("Quickest cowboy of Vikendi.\n(50DMG)");
                            if (itemName == "Revolver") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 10) {
                            ui_inventoryHint.setString("Boom! Headshot!\n(35DMG)");
                            if (itemName == "Crossbow") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }
                        if (uiInventorySelection == 11) {
                            ui_inventoryHint.setString("Fills you with determination.\n(+100HP)");
                            if (itemName == "Health Potion") uiInventory[i].Container.setFillColor(Color(0, 162, 232));
                            else uiInventory[i].Container.setFillColor(Color(35, 35, 35));
                        }

                        if (PlayerFile["player_weapon_equipped"] == 5) {
                            if (itemName == "Sword") uiInventory[i].Amount.setString("Equipped");
                            else if (uiInventory[i].itemType == "weapon") uiInventory[i].Amount.setString("");
                        }
                        if (PlayerFile["player_weapon_equipped"] == 6) {
                            if (itemName == "Eyelander") uiInventory[i].Amount.setString("Equipped");
                            else if (uiInventory[i].itemType == "weapon") uiInventory[i].Amount.setString("");
                        }
                        if (PlayerFile["player_weapon_equipped"] == 7) {
                            if (itemName == "Spear") uiInventory[i].Amount.setString("Equipped");
                            else if (uiInventory[i].itemType == "weapon") uiInventory[i].Amount.setString("");
                        }
                        if (PlayerFile["player_weapon_equipped"] == 8) {
                            if (itemName == "Magic Staff") uiInventory[i].Amount.setString("Equipped");
                            else if (uiInventory[i].itemType == "weapon") uiInventory[i].Amount.setString("");
                        }
                        if (PlayerFile["player_weapon_equipped"] == 9) {
                            if (itemName == "Revolver") uiInventory[i].Amount.setString("Equipped");
                            else if (uiInventory[i].itemType == "weapon") uiInventory[i].Amount.setString("");
                        }
                        if (PlayerFile["player_weapon_equipped"] == 10) {
                            if (itemName == "Crossbow") uiInventory[i].Amount.setString("Equipped");
                            else if (uiInventory[i].itemType == "weapon") uiInventory[i].Amount.setString("");
                        }
                    }
                }

                if (isPaused) {
                    button_menuSelection1.setSize(Vector2f(200, 35));
                    uiBlur.setFillColor(Color(255, 255, 255, 70));
                    text_menuSelection1.setString("Resume Game");
                    if (!isinCombat && introCutscenePlayed) text_menuSelection2.setString("Save Game");
                    else text_menuSelection2.setString("Save Unavailable");
                    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                        dialogue.play();
                        uiMenuSelection++;
                        uiTick.restart();
                    }
                    else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                        dialogue.play();
                        uiMenuSelection--;
                        uiTick.restart();
                    }

                    if (Joystick::isButtonPressed(0, 1)) {
                        isPaused = false;
                        uiBlur.setFillColor(Color(0, 0, 0, 0));
                    }

                    button_menuSelection1.setFillColor(Color(35, 35, 35));
                    button_menuSelection2.setFillColor(Color(35, 35, 35));
                    button_menuSelection3.setFillColor(Color(35, 35, 35));

                    if (uiMenuSelection > 3) uiMenuSelection = 1;
                    if (uiMenuSelection < 1) uiMenuSelection = 3;
                    if (uiMenuSelection == 1) {
                        button_menuSelection1.setFillColor(Color(0, 162, 232));
                        text_menuHint.setString("Back to adventure!");

                        if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                            isPaused = false;
                            uiBlur.setFillColor(Color(0, 0, 0, 0));
                            uiTick.restart();
                        }
                    }
                    else if (uiMenuSelection == 2) {
                            if (!isinCombat && introCutscenePlayed) button_menuSelection2.setFillColor(Color(0, 162, 232));
                            else button_menuSelection2.setFillColor(Color(196, 0, 30));

                            if (isSaving) text_menuHint.setString("Save success!");
                            else if (!isinCombat && introCutscenePlayed) text_menuHint.setString("Save your progress.");
                            else text_menuHint.setString("Can't save right now.");

                            if (isSaving && uiTick.getElapsedTime().asSeconds() > 2) isSaving = false;

                            if (!isinCombat && introCutscenePlayed) {
                                if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                                    // Save game
                                    save.play();
                                    remove("Assets\\save.txt");
                                    ofstream newSave("Assets\\save.txt");
                                    newSave << "Save file. Editing this save file may cause some.... unforseen consequences...\n";
                                    newSave << "player_sprite_selected " << uiCharacterSelected << "\n";
                                    newSave << "player_weapon_equipped " << PlayerFile["player_weapon_equipped"] << "\n";
                                    for (int i = 0; i != uiInventory.size(); i++) {
                                        if (uiInventory[i].itemName == "Apple") newSave << "inv_apple_amt " << PlayerFile["inv_apple_amt"] << "\n";
                                        if (uiInventory[i].itemName == "Special Apple") newSave << "inv_spcapple_amt " << PlayerFile["inv_spcapple_amt"] << "\n";
                                        if (uiInventory[i].itemName == "Meat") newSave << "inv_meat_amt " << PlayerFile["inv_meat_amt"] << "\n";
                                        if (uiInventory[i].itemName == "Herb") newSave << "inv_herb_amt " << PlayerFile["inv_herb_amt"] << "\n";
                                        if (uiInventory[i].itemName == "Health Potion") newSave << "inv_hpotion_amt " << PlayerFile["inv_hpotion_amt"] << "\n";
                                        if (uiInventory[i].itemName == "Health") newSave << "inv_health " << PlayerFile["inv_health"] << "\n";
                                        if (uiInventory[i].itemName == "Gold") newSave << "inv_gold " << PlayerFile["inv_gold"] << "\n";
                                    }
                                    newSave << "player_x " << spr_player.getPosition().x << "\n";
                                    newSave << "player_y " << spr_player.getPosition().y << "\n";
                                    newSave << "view_x " << mainView.getCenter().x << "\n";
                                    newSave << "view_y " << mainView.getCenter().y << "\n";
                                    newSave << "lastarea " << OOBCheck << "\n";
                                    newSave.close();
                                    isSaving = true;
                                    uiTick.restart();
                                }
                            }
                        }
                        else if (uiMenuSelection == 3) {
                        button_menuSelection3.setFillColor(Color(0, 162, 232));
                        text_menuHint.setString("Remember to save, as you will lose all unsaved progress.");

                        if (Keyboard::isKeyPressed(Keyboard::LShift) || abs(Joystick::getAxisPosition(0, Joystick::Z)) > 50) {
                            if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) app.close();
                        }
                        else {
                            if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) { // Exit to menu
                                isPaused = false;
                                isPlaying = false;
                                isInDialogue = false;
                                isinInventory = false;
                                isinShop = false;
                                isinCombat = false;
                                worldMusicPlaying = false;
                                world1Music.stop();
                                firstIntroBlur = false;
                                spr_player.setPosition(PlayerFile["player_x"], PlayerFile["player_y"]);
                                mainView.setCenter(PlayerFile["view_x"], PlayerFile["view_y"]);
                                footstepCounter = 0;
                                currentDialogue = 0;
                                uiMenuSelection = 1;
                                uiCharacterSelected = 1;
                                uiInventorySelection = 1;
                                uiMenuStyle = rand() % 2;
                                if (uiMenuStyle == 0) {
                                    menuAnim.setTexture(&menuImage);
                                }
                                else menuAnim.setTexture(&menuImage2);
                                uiBlur.setFillColor(Color(0, 0, 0, 255));
                                uiTick.restart();
                            }
                        }
                    }
                    if (Keyboard::isKeyPressed(Keyboard::LShift) || abs(Joystick::getAxisPosition(0, Joystick::Z)) > 50) text_menuSelection3.setString("Exit Game");
                    else text_menuSelection3.setString("Return to Menu");
                }
            }
        }
        else { // Main menu
            button_menuSelection1.setSize(Vector2f(200, 35));
            if (menuMusic.getStatus() == Music::Stopped && uiMenuStyle == 0) {
                menuMusic.setVolume(100);
                menuMusic.play();
            }
            if (menuMusic2.getStatus() == Music::Stopped && uiMenuStyle == 1) {
                menuMusic2.setVolume(100);
                menuMusic2.play();
            }
            if (uiBlur.getFillColor().a > 0 && !isStartingGame && !isShowingBinds) uiBlur.setFillColor(Color(0, 0, 0, uiBlur.getFillColor().a - 1));
            menuAnim.setTextureRect(IntRect((frameTick.getElapsedTime().asMilliseconds()/100 % 6) * 512, 0, 512, 317));
            if (saveExists) text_menuSelection1.setString("Load Game");
            else text_menuSelection1.setString("New Game");
            text_menuSelection2.setString("View Controls");
            text_menuSelection3.setString("Exit Game");

            if (!isSelectingCharacter && !isShowingBinds && uiTick.getElapsedTime().asSeconds() > 0.25 && !isStartingGame) {
                if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S) || Joystick::isButtonPressed(0, 11) || ctrl_y > 50) {
                    uiMenuSelection++;
                    uiTick.restart();
                }
                else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W) || Joystick::isButtonPressed(0, 12) || ctrl_y < -50) {
                    uiMenuSelection--;
                    uiTick.restart();
                }

                if (uiMenuSelection > 3) uiMenuSelection = 1;
                if (uiMenuSelection < 1) uiMenuSelection = 3;
                if (uiMenuSelection == 1) {
                    button_menuSelection1.setFillColor(Color(0, 162, 232));
                    button_menuSelection2.setFillColor(Color(35, 35, 35));
                    button_menuSelection3.setFillColor(Color(35, 35, 35));
                    if (saveExists) text_menuHint.setString("Load your last save");
                    else text_menuHint.setString("Start a new game");

                    if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                        if (!saveExists) {
                            text_menuHint.setString("Select character");
                            isSelectingCharacter = true;
                            uiTick.restart();
                        }
                        else {
                            // Load save file
                            string var;
                            int val;
                            ifstream savefile;
                            savefile.open("Assets\\save.txt");
                            savefile.ignore(1000, '\n');
                            if (savefile.is_open()) {
                                saveExists = true;
                                while (savefile >> var >> val) {
                                    PlayerFile[var] = val;
                                    if (PlayerFile["player_sprite_selected"] == 1) playerSpriteOffset = 0;
                                    else if (PlayerFile["player_sprite_selected"] == 2) playerSpriteOffset = 78;
                                    else if (PlayerFile["player_sprite_selected"] == 3) playerSpriteOffset = 156;
                                    else if (PlayerFile["player_sprite_selected"] == 4) playerSpriteOffset = 234;

                                    OOBCheck = PlayerFile["lastarea"];
                                    uiCharacterSelected = PlayerFile["player_sprite_selected"];
                                    spr_player.setPosition(Vector2f(PlayerFile["player_x"], PlayerFile["player_y"]));
                                    mainView.setCenter(PlayerFile["view_x"], PlayerFile["view_y"]);
                                }
                            }
                            savefile.close();
                            // Check for save file modifications
                            if (PlayerFile["player_sprite_selected"] < 1 || PlayerFile["player_sprite_selected"] > 4
                                || PlayerFile["player_weapon_equipped"] > 10 || PlayerFile["inv_apple_amt"] < 0
                                || PlayerFile["inv_spcapple_amt"] < 0 || PlayerFile["inv_meat_amt"] < 0
                                || PlayerFile["inv_herb_amt"] < 0 || PlayerFile["inv_hpotion_amt"] < 0
                                || PlayerFile["inv_health"] < 0 || PlayerFile["inv_gold"] < 0
                                || PlayerFile["inv_health"] > 100 || PlayerFile["lastarea"] < 1 || PlayerFile["lastarea"] > 3) {
                                PlayerFile["player_weapon_equipped"] = 5; // Defaults player variables
                                PlayerFile["inv_health"] = 100;
                                PlayerFile["inv_gold"] = 0;
                                PlayerFile["player_x"] = 4093;
                                PlayerFile["player_y"] = 8055;
                                PlayerFile["view_x"] = 4119;
                                PlayerFile["view_y"] = 7569;
                                PlayerFile["inv_apple_amt"] = 1;
                                PlayerFile["inv_spcapple_amt"] = 1;
                                PlayerFile["inv_meat_amt"] = 1;
                                PlayerFile["inv_herb_amt"] = 1;
                                PlayerFile["inv_hpotion_amt"] = 1;
                                button_menuSelection1.setFillColor(Color(196, 0, 30));
                                text_menuHint.setString("Invalid values for\nsave file detected.\nSelect character");
                                isSelectingCharacter = true;
                                uiTick.restart();
                            }
                            if (!isSelectingCharacter) {
                                isStartingGame = true;
                                uiTick.restart();
                            }
                        }
                    }
                }
                else if (uiMenuSelection == 2) {
                    button_menuSelection2.setFillColor(Color(0, 162, 232));
                    button_menuSelection1.setFillColor(Color(35, 35, 35));
                    button_menuSelection3.setFillColor(Color(35, 35, 35));
                    text_menuHint.setString("Check controls for Keyboard or Controller");

                    if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                        isShowingBinds = true;
                        uiTick.restart();
                    }
                }
                else if (uiMenuSelection == 3) {
                    button_menuSelection1.setFillColor(Color(35, 35, 35));
                    button_menuSelection2.setFillColor(Color(35, 35, 35));
                    button_menuSelection3.setFillColor(Color(0, 162, 232));
                    text_menuHint.setString("Exit the game");

                    if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) app.close();
                }
            }

            if (isShowingBinds) {
                if (uiBlur.getFillColor().a < 150) uiBlur.setFillColor(Color(0, 0, 0, uiBlur.getFillColor().a + 1));
                if (Keyboard::isKeyPressed(Keyboard::Escape) || Joystick::isButtonPressed(0, 1)) {
                    isShowingBinds = false;
                }
            }

            if (isStartingGame) {
                int spritePos = ((frameTick.getElapsedTime().asMilliseconds()/6 % 3) * 26) + playerSpriteOffset;
                spr_player.setTextureRect(IntRect(spritePos, 0, 26, 36));
                if (uiBlur.getFillColor().a < 255) uiBlur.setFillColor(Color(0, 0, 0, uiBlur.getFillColor().a + 1));
                else {
                    isStartingGame = false;
                    menuMusic.stop();
                    menuMusic2.stop();
                    isPlaying = true;
                }

                if (menuMusic.getVolume() > 1) menuMusic.setVolume(menuMusic.getVolume() - 1);
                if (menuMusic2.getVolume() > 1) menuMusic2.setVolume(menuMusic2.getVolume() - 1);
            }

            if (isSelectingCharacter && uiTick.getElapsedTime().asSeconds() > 0.15) {
                button_char1.setFillColor(Color(35, 35, 35));
                button_char2.setFillColor(Color(35, 35, 35));
                button_char3.setFillColor(Color(35, 35, 35));
                button_char4.setFillColor(Color(35, 35, 35));
                if (uiCharacterSelected == 1) {
                    playerSpriteOffset = 0;
                    button_char1.setFillColor(Color(0, 162, 232));
                }
                else if (uiCharacterSelected == 2) {
                    playerSpriteOffset = 78;
                    button_char2.setFillColor(Color(0, 162, 232));
                }
                else if (uiCharacterSelected == 3) {
                    playerSpriteOffset = 156;
                    button_char3.setFillColor(Color(0, 162, 232));
                }
                else if (uiCharacterSelected == 4) {
                    playerSpriteOffset = 234;
                    button_char4.setFillColor(Color(0, 162, 232));
                }

                if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A) || ctrl_x < -50) {
                    uiCharacterSelected--;
                    uiTick.restart();
                }
                if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D) || ctrl_x > 50) {
                    uiCharacterSelected++;
                    uiTick.restart();
                }

                if (Keyboard::isKeyPressed(Keyboard::Escape) || Joystick::isButtonPressed(0, 1)) {
                    uiCharacterSelected = 1;
                    isSelectingCharacter = false;
                    uiTick.restart();
                }

                if (Keyboard::isKeyPressed(Keyboard::Return) || Joystick::isButtonPressed(0, 0)) {
                        isSelectingCharacter = false;
                        isStartingGame = true;
                        uiTick.restart();
                    }

                if (uiCharacterSelected < 1) uiCharacterSelected = 4;
                if (uiCharacterSelected > 4) uiCharacterSelected = 1;
            }
        }

        app.clear(Color(215, 231, 229));
        if (isPlaying) { // main game run
            app.setView(mainView);
            app.draw(dimerMap);
            if (isinCombat && footstepTick.getElapsedTime().asSeconds() > 0 && footstepTick.getElapsedTime().asSeconds() < 1 && !isFleeingCombat) { // Encounter
                app.setView(app.getDefaultView());
                app.draw(encounterBox);
            }

            if (isFleeingCombat || isinCombat && footstepTick.getElapsedTime().asSeconds() > 1) { // In Combat
                app.setView(app.getDefaultView());
                app.draw(menuBox);
                app.draw(winterBack);
                app.draw(ui_combatPlayerTag);
                app.draw(text_combatPlayerTag);
                app.draw(ui_combatPlayerHealth.Container);
                app.draw(ui_combatPlayerHealth.Icon);
                app.draw(ui_combatPlayerHealth.Label);
                app.draw(ui_combatPlayerHealth.Amount);
                app.draw(ui_combatEnemyTag);
                app.draw(text_combatEnemyTag);
                app.draw(ui_combatEnemyHealth.Container);
                app.draw(ui_combatEnemyHealth.Icon);
                app.draw(ui_combatEnemyHealth.Label);
                app.draw(ui_combatEnemyHealth.Amount);

                if (isCombatInventory || isCombatSelection) { // Combat inventory selection
                    if (!isCombatPlayerAttacking && !isCombatEnemyAttacking && !enemyDefeated && !playerDefeated && !playerHealed) {
                        if (isCombatInventory) {
                            for (int i = 0; i != combatInventory.size(); i++) {
                                app.draw(combatInventory[i].Container);
                                app.draw(combatInventory[i].Icon);
                                app.draw(combatInventory[i].Label);
                                app.draw(combatInventory[i].Amount);
                            }
                            app.draw(text_combatInventoryHint);
                            app.draw(text_combatInvTitle);
                            app.draw(text_combatEsc);
                        }

                        if (isCombatSelection) { // Combat move selection
                            app.draw(text_combatFightTitle);
                            app.draw(text_combatEsc);
                            app.draw(text_combatWeapon);
                            app.draw(button_combatFightMove1);
                            app.draw(button_combatFightMove2);
                            app.draw(button_combatFightMove3);
                            app.draw(text_combatFightMove1);
                            app.draw(text_combatFightMove2);
                            app.draw(text_combatFightMove3);
                        }
                    }
                }
                else if (!playerHealed && !isCombatPlayerAttacking && !isCombatEnemyAttacking) { // Combat menu selection
                    app.draw(button_combatSelection1);
                    app.draw(button_combatSelection2);
                    app.draw(button_combatSelection3);
                    app.draw(text_combatSelection1);
                    app.draw(text_combatSelection2);
                    app.draw(text_combatSelection3);
                    app.draw(text_combatHint);
                }
            }
            app.setView(mainView);
            app.draw(spr_player);
            if (isinCombat) {
                app.draw(enemy.spr_npc);
                if (isCombatPlayerAttacking || isCombatEnemyAttacking || playerHealed) {
                    app.draw(combatEffect);
                    app.draw(text_combatDamageAmount);
                }
            }
            if (enemyDefeated || playerDefeated) {
                app.setView(app.getDefaultView());
                if (playerDefeated) {
                    app.draw(uiBlur);
                }
                app.draw(ui_combatText);
            }
            if (!isFleeingCombat) {
                if (!isinCombat || isinCombat && footstepTick.getElapsedTime().asSeconds() < 1) {
                    for (int i = 0; i != spr_npcs.size(); i++) {
                        app.draw(spr_npcs[i].spr_npc);
                        app.draw(spr_npcs[i].text_dialoguetrigger);
                    }
                    app.setView(app.getDefaultView());
                    if (!isinInventory && !isInDialogue && !isinShop) {
                        app.draw(ui_GeneralHelp);
                        app.draw(text_helpHealth);
                        app.draw(text_helpGold);
                    }

                    if (isinShop) { // Shop
                        app.draw(ui_shopBox);
                        app.draw(text_shopHeader);
                        app.draw(text_shopExit);
                        for (int i = 0; i != shopInventory.size(); i++) {
                            app.draw(shopInventory[i].Container);
                            app.draw(shopInventory[i].Icon);
                            app.draw(shopInventory[i].Label);
                            app.draw(shopInventory[i].Amount);
                        }
                    }
                }
            }
            if (!firstIntroBlur || isPlaying && !isinCombat) app.draw(uiBlur);
        }
        else { // Main menu elements
            app.setView(app.getDefaultView());
            app.draw(menuBox);
            app.draw(menuAnim);
            app.draw(button_menuSelection1);
            app.draw(button_menuSelection2);
            app.draw(button_menuSelection3);
            app.draw(text_menuSelection1);
            app.draw(text_menuSelection2);
            app.draw(text_menuSelection3);
            if (isSelectingCharacter) { // Character selection
                app.draw(button_char1);
                app.draw(button_char2);
                app.draw(button_char3);
                app.draw(button_char4);
                app.draw(menu_char1);
                app.draw(menu_char2);
                app.draw(menu_char3);
                app.draw(menu_char4);
            }
            app.draw(text_menuHint);
            app.draw(text_credit);
            app.draw(uiBlur);
            if (isShowingBinds) {
                if (inputMethod == "kb") app.draw(menuKeybindsKB);
                if (inputMethod == "ctrl") app.draw(menuKeybindsCTRL);
            }
        }

        if (isInDialogue) { // Dialogue box
            app.setView(app.getDefaultView());
            app.draw(ui_dialogueBox);
            app.draw(ui_dialogueText);
            app.draw(ui_dialogueSpeaker);
            app.draw(ui_dialogueHint);
        }
        if (isinInventory) { // Inventory (tab)
            app.setView(app.getDefaultView());
            app.draw(ui_inventoryBox);
            app.draw(ui_inventoryHeader);
            app.draw(ui_inventorySnacks);
            app.draw(ui_inventoryWeapons);
            app.draw(ui_inventoryPotions);
            app.draw(ui_inventoryStats);
            app.draw(ui_inventoryHint);

            for (int i = 0; i != uiInventory.size(); i++) {
                app.draw(uiInventory[i].Container);
                app.draw(uiInventory[i].Icon);
                app.draw(uiInventory[i].Amount);
                app.draw(uiInventory[i].Label);
            }
        }
        if (isPaused) { // Pause menu
            app.setView(app.getDefaultView());
            app.draw(uiBlur);
            app.draw(menuBox);
            app.draw(button_menuSelection1);
            app.draw(button_menuSelection2);
            app.draw(button_menuSelection3);
            app.draw(text_menuSelection1);
            app.draw(text_menuSelection2);
            app.draw(text_menuSelection3);
            app.draw(text_menuHint);
            app.draw(text_credit);
        }
        app.display();
    }

    return 0;
}
