% TURING MIAMI
% based off of Hotline Miami
% by Brandon Luu 2018

% WASD to move
% Hold G to knife

% Sprites from Hotline Miami, and all levels were custom made by me
put "Loading"

% Keyboard Input Variables
var PlayerX : int := 480 % Player X and Y coordinates
var PlayerY : int := -60
var PlayerDied : int := 0 % Whether or not Player is alive
var KeyboardChars : array char of boolean % Keyboard character mapping
var MovementIncrement : int := 3 % X,Y movement increments
var PlayerFacing : string := "up" % Direction player is facing
var PlayerAction : string := "walk" % What the player is doing

var NPCAnimTick : int := 0 % Proper timing for NPCs
var MovementEnabled : int := 0 % Whether movement is enabled or not

% Graphics Setup/Variables
View.Set ("graphics:1024, 768, nobuttonbar, offscreenonly, title:TURING MIAMI")
var DelayTime : int % Variable needed for FileNewFrames
var FrameSpeed : int := 9 % Frame speed for sprites
var PlayerCurrentAnim : int % Player sprite's current pic/animation

% Player Sprites
var PlayerWalkFrames : int := Pic.Frames ("Graphics/Player/PlayerWalk.gif") % Create sprite for Player walking
var PlayerWalkPics : array 1 .. PlayerWalkFrames of int
Pic.FileNewFrames ("Graphics/Player/PlayerWalk.gif", PlayerWalkPics, DelayTime)
var PlayerWalkSprite : int := Sprite.New (PlayerWalkPics (1))

var PlayerDeadFrames : int := Pic.Frames ("Graphics/Player/PlayerDead.gif") % Create sprite for Player death
var PlayerDeadPics : array 1 .. PlayerDeadFrames of int
Pic.FileNewFrames ("Graphics/Player/PlayerDead.gif", PlayerDeadPics, DelayTime)
var PlayerDeadSprite : int := Sprite.New (PlayerDeadPics (1))

var PlayerKnifeFrames : int := Pic.Frames ("Graphics/Player/PlayerKnife.gif") % Create sprite for Player stabbing
var PlayerKnifePics : array 1 .. PlayerKnifeFrames of int
Pic.FileNewFrames ("Graphics/Player/PlayerKnife.gif", PlayerKnifePics, DelayTime)
var PlayerKnifeSprite : int := Sprite.New (PlayerKnifePics (1))

% Guard (Bat) Sprite
var GuardBat1SwingFrames : int := Pic.Frames ("Graphics/Enemies/Guard (Bat)/GuardBatSwing.gif") % Create sprite and variables for Guard #1
var GuardBat1SwingPics : array 1 .. GuardBat1SwingFrames of int
Pic.FileNewFrames ("Graphics/Enemies/Guard (Bat)/GuardBatSwing.gif", GuardBat1SwingPics, DelayTime)
var GuardBat1SwingSprite : int := Sprite.New (GuardBat1SwingPics (1))
var GuardBat1X := 0
var GuardBat1Y := 0
var GuardBat1Alerted : int := 0
var GuardBat1Dead : int := 0
var GuardBat1Direction : string := "left"

var GuardBat2SwingSprite : int := Sprite.New (GuardBat1SwingPics (1)) % Create sprite and variables for Guard #2
var GuardBat2X : int := 0
var GuardBat2Y : int := 0
var GuardBat2Alerted : int := 0
var GuardBat2Dead : int := 0
var GuardBat2Direction : string := "left"

var GuardBat3SwingSprite : int := Sprite.New (GuardBat1SwingPics (1)) % Create sprite and variables for Guard #3
var GuardBat3X : int := 0
var GuardBat3Y : int := 0
var GuardBat3Alerted : int := 0
var GuardBat3Dead : int := 0
var GuardBat3Direction : string := "left"

var GuardBat4SwingSprite : int := Sprite.New (GuardBat1SwingPics (1)) % Create sprite and variables for Guard #4
var GuardBat4X : int := 0
var GuardBat4Y : int := 0
var GuardBat4Alerted : int := 0
var GuardBat4Dead : int := 0
var GuardBat4Direction : string := "left"

% Guard #5 is final boss/sprite guy
var GuardBat5SwingSprite : int := Sprite.New (GuardBat1SwingPics (1)) % Create sprite and variables for Guard #5
var GuardBat5X : int := 0
var GuardBat5Y : int := 0
var GuardBat5Alerted : int := 0
var GuardBat5Dead : int := 0
var GuardBat5Direction : string := "down"

var GuardBatDeadFrames : int := Pic.Frames ("Graphics/Enemies/Guard (Bat)/GuardBatDead.gif") % Guard dead sprites
var GuardBatDeadPics : array 1 .. GuardBatDeadFrames of int
Pic.FileNewFrames ("Graphics/Enemies/Guard (Bat)/GuardBatDead.gif", GuardBatDeadPics, DelayTime)
var GuardBatDeadSprite : int := Sprite.New (GuardBatDeadPics (1))
var GuardBat2DeadSprite : int := Sprite.New (GuardBatDeadPics (1))
var GuardBat3DeadSprite : int := Sprite.New (GuardBatDeadPics (1))
var GuardBat4DeadSprite : int := Sprite.New (GuardBatDeadPics (1))
var GuardBat5DeadSprite : int := Sprite.New (GuardBatDeadPics (1))

% Level Variables
var CurrentLevel : int := 0 % Current level variable
var DialogueTick : int := 0 % Dialogue currently in use
var DialogueDone : int := 0 % Whether or not the dialogue is finished

var StartupScreen : int := Pic.FileNew ("Graphics/Misc/Startup.jpg") % Startup screen image
var GameStart : int := 0 % Game started or not

% Level: Tutorial
var Level00CutscenePlayed : int := 0 % Cutscene variable and load up images of level 0
var Level00 : int := Pic.FileNew ("Graphics/Levels/0/Room.jpg")
var Level00Bars : int := Pic.FileNew ("Graphics/Levels/0/RoomBars.jpg")
var Level00Text1 : int := Pic.FileNew ("Graphics/Levels/0/RoomText1.jpg")
var Level00Text2 : int := Pic.FileNew ("Graphics/Levels/0/RoomText2.jpg")
var Level00Text3 : int := Pic.FileNew ("Graphics/Levels/0/RoomText3.jpg")
var Level00Text4 : int := Pic.FileNew ("Graphics/Levels/0/RoomText4.jpg")
var Level00Text5 : int := Pic.FileNew ("Graphics/Levels/0/RoomText5.jpg")
var Level00Text6 : int := Pic.FileNew ("Graphics/Levels/0/RoomText6.jpg")

% Level: 1
var Level01CutscenePlayed : int := 0 % Cutscene variable and load up images of level 1
var Level01 : int := Pic.FileNew ("Graphics/Levels/1/Room.jpg")
var Level01Died : int := Pic.FileNew ("Graphics/Levels/1/RoomDied.jpg")

% Level: 2
var Level02CutscenePlayed : int := 0 % Cutscene variable and load up images of level 2
var Level02 : int := Pic.FileNew ("Graphics/Levels/2/Room.jpg")
var Level02Died : int := Pic.FileNew ("Graphics/Levels/2/RoomDied.jpg")
var Level02Gun : int := Pic.FileNew ("Graphics/Levels/2/RoomGun.jpg")

% Level: 3
var Level03CutscenePlayed : int := 0 % Cutscene variable and load up images of level 3
var Level03 : int := Pic.FileNew ("Graphics/Levels/3/Room.jpg")
var Level03Died : int := Pic.FileNew ("Graphics/Levels/3/RoomDied.jpg")
var Level03Text1 : int := Pic.FileNew ("Graphics/Levels/3/RoomText1.jpg")
var Level03Text2 : int := Pic.FileNew ("Graphics/Levels/3/RoomText2.jpg")
var Level03Text3 : int := Pic.FileNew ("Graphics/Levels/3/RoomText3.jpg")
var Level03Text4 : int := Pic.FileNew ("Graphics/Levels/3/RoomText4.jpg")
var Level03Text5 : int := Pic.FileNew ("Graphics/Levels/3/RoomText5.jpg")
var Level03Text6 : int := Pic.FileNew ("Graphics/Levels/3/RoomText6.jpg")
var Level03Text7 : int := Pic.FileNew ("Graphics/Levels/3/RoomText7.jpg")
var EndGame : int := 0

procedure StartLevel01 () % (Re)Start Level 1 when player dies/enters room
    Sprite.Hide (GuardBatDeadSprite)
    Sprite.Hide (GuardBat2DeadSprite)
    Sprite.Hide (GuardBat3DeadSprite)
    Sprite.Hide (GuardBat4DeadSprite)
    Sprite.Hide (PlayerDeadSprite)
    Sprite.Show (PlayerWalkSprite)
    Sprite.Show (GuardBat1SwingSprite)
    Sprite.Show (GuardBat2SwingSprite)
    Sprite.Show (GuardBat3SwingSprite)
    Sprite.Show (GuardBat4SwingSprite)
    GuardBat1Dead := 0
    GuardBat2Dead := 0
    GuardBat3Dead := 0
    GuardBat4Dead := 0
    MovementEnabled := 0
    PlayerDied := 0
    PlayerX := 45
    PlayerY := -20
    randint (GuardBat1X, 135, 700)
    randint (GuardBat1Y, 70, 250)
    randint (GuardBat3X, 135, 700)
    randint (GuardBat3Y, 70, 250)
    randint (GuardBat4X, 135, 700)
    randint (GuardBat4Y, 350, 650)
    GuardBat1Alerted := 0
    GuardBat2Alerted := 0
    GuardBat3Alerted := 0
    GuardBat4Alerted := 0
    GuardBat2X := 100
    GuardBat2Y := 500
    PlayerFacing := "up"
    GuardBat1Direction := "left"
    GuardBat2Direction := "right"
    GuardBat3Direction := "left"
    GuardBat4Direction := "right"
    Level01CutscenePlayed := 0
    CurrentLevel := 1
end StartLevel01

procedure StartLevel02 () % (Re)Start Level 2 when player dies/enters room
    Sprite.Hide (GuardBatDeadSprite)
    Sprite.Hide (GuardBat2DeadSprite)
    Sprite.Hide (GuardBat3DeadSprite)
    Sprite.Hide (GuardBat4DeadSprite)
    Sprite.Hide (GuardBat5DeadSprite)
    Sprite.Hide (PlayerDeadSprite)
    Sprite.Show (PlayerWalkSprite)
    Sprite.Show (GuardBat1SwingSprite)
    Sprite.Show (GuardBat2SwingSprite)
    Sprite.Show (GuardBat3SwingSprite)
    Sprite.Show (GuardBat4SwingSprite)
    Sprite.Show (GuardBat5SwingSprite)
    PlayerX := 1054
    PlayerY := 600
    GuardBat1Dead := 0
    GuardBat2Dead := 0
    GuardBat3Dead := 0
    GuardBat4Dead := 0
    GuardBat5Dead := 0
    MovementEnabled := 0
    PlayerDied := 0
    randint (GuardBat1X, 10, 800)
    randint (GuardBat1Y, 30, 550)
    randint (GuardBat3X, 10, 800)
    randint (GuardBat3Y, 30, 550)
    randint (GuardBat4X, 10, 880)
    randint (GuardBat4Y, 30, 550)
    randint (GuardBat5X, 10, 880)
    randint (GuardBat5Y, 30, 550)
    GuardBat1Alerted := 0
    GuardBat2Alerted := 0
    GuardBat3Alerted := 0
    GuardBat4Alerted := 0
    GuardBat5Alerted := 0
    PlayerFacing := "left"
    GuardBat1Direction := "left"
    GuardBat2Direction := "right"
    GuardBat3Direction := "left"
    GuardBat4Direction := "right"
    GuardBat5Direction := "right"
    Level02CutscenePlayed := 0
    CurrentLevel := 2
end StartLevel02

procedure StartLevel03 () % (Re)Start Level 3 when player dies/enters room
    DialogueDone := 0
    DialogueTick := 0
    Sprite.Hide (GuardBatDeadSprite)
    Sprite.Hide (GuardBat2DeadSprite)
    Sprite.Hide (GuardBat3DeadSprite)
    Sprite.Hide (GuardBat4DeadSprite)
    Sprite.Hide (GuardBat5DeadSprite)
    Sprite.Hide (PlayerDeadSprite)
    Sprite.Show (PlayerWalkSprite)
    Sprite.Show (GuardBat1SwingSprite)
    Sprite.Show (GuardBat2SwingSprite)
    Sprite.Show (GuardBat3SwingSprite)
    Sprite.Show (GuardBat4SwingSprite)
    Sprite.Show (GuardBat5SwingSprite)
    PlayerX := 480
    PlayerY := -60
    GuardBat1Dead := 0
    GuardBat2Dead := 0
    GuardBat3Dead := 0
    GuardBat4Dead := 0
    GuardBat5Dead := 0
    MovementEnabled := 0
    PlayerDied := 0
    GuardBat1X := 20
    GuardBat1Y := 40
    GuardBat2X := 875
    GuardBat2Y := 40
    GuardBat3X := 20
    GuardBat3Y := 550
    GuardBat4X := 875
    GuardBat4Y := 550
    GuardBat5X := 430
    GuardBat5Y := 520
    GuardBat1Alerted := 0
    GuardBat2Alerted := 0
    GuardBat3Alerted := 0
    GuardBat4Alerted := 0
    GuardBat5Alerted := 0
    PlayerFacing := "up"
    GuardBat1Direction := "left"
    GuardBat2Direction := "right"
    GuardBat3Direction := "down"
    GuardBat4Direction := "down"
    GuardBat5Direction := "down"
    Level03CutscenePlayed := 0
    CurrentLevel := 3
end StartLevel03

% Main Loop
loop
    cls
    Input.KeyDown (KeyboardChars) % Get keyboard chars

    % Keyboard Inputs for WASD, and G
    if MovementEnabled = 1 and PlayerDied = 0 then
	if KeyboardChars ('d') then
	    PlayerAction := "walk"
	    PlayerFacing := "right"
	    PlayerX := PlayerX + MovementIncrement
	end if

	if KeyboardChars ('a') then
	    PlayerAction := "walk"
	    PlayerFacing := "left"
	    PlayerX := PlayerX - MovementIncrement
	end if

	if KeyboardChars ('w') then
	    PlayerAction := "walk"
	    PlayerFacing := "up"
	    PlayerY := PlayerY + MovementIncrement
	end if

	if KeyboardChars ('s') then
	    PlayerAction := "walk"
	    PlayerFacing := "down"
	    PlayerY := PlayerY - MovementIncrement
	end if

	if KeyboardChars ('g') then
	    PlayerAction := "knife"
	end if
    else
	if GameStart = 0 then
	    Pic.Draw(StartupScreen, 0, 0, picCopy)
	    if KeyboardChars (KEY_ENTER) then
		GameStart := 1
	    end if
	else
	    if DialogueDone = 1 then
		if KeyboardChars (KEY_ENTER) then
		    delay (30)
		    DialogueTick := DialogueTick + 1
		    DialogueDone := 0
		end if
	    end if

	    if PlayerDied = 1 then
		if KeyboardChars (KEY_ENTER) then
		    if CurrentLevel = 1 then
			StartLevel01 ()
		    elsif CurrentLevel = 2 then
			StartLevel02 ()
		    elsif CurrentLevel = 3 then
			StartLevel03 ()
		    end if
		end if
	    end if
	end if
    end if

    if not NPCAnimTick > 150 then % NPC Animation Tickrate, so the animation doesn't mess up and for "hit" detection
	NPCAnimTick := NPCAnimTick + 2
    else
	NPCAnimTick := 0
    end if

    % Player Animations
    if PlayerAction = "walk" then % Walk when using WASD and animate
	Sprite.Show (PlayerWalkSprite)
	Sprite.Hide (PlayerKnifeSprite)
	if PlayerFacing = "right" then
	    PlayerCurrentAnim := (PlayerWalkPics (PlayerX div FrameSpeed mod PlayerWalkFrames + 1))
	    Sprite.Animate (PlayerWalkSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	elsif PlayerFacing = "left" then
	    PlayerCurrentAnim := Pic.Mirror (PlayerWalkPics (PlayerX div FrameSpeed mod PlayerWalkFrames + 1))
	    Sprite.Animate (PlayerWalkSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	    Pic.Free (PlayerCurrentAnim)
	elsif PlayerFacing = "up" then
	    PlayerCurrentAnim := Pic.Rotate (PlayerWalkPics (PlayerY div FrameSpeed mod PlayerWalkFrames + 1), 90, 40, 40)
	    Sprite.Animate (PlayerWalkSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	    Pic.Free (PlayerCurrentAnim)
	elsif PlayerFacing = "down" then
	    PlayerCurrentAnim := Pic.Rotate (PlayerWalkPics (PlayerY div FrameSpeed mod PlayerWalkFrames + 1), -90, 40, 40)
	    Sprite.Animate (PlayerWalkSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	    Pic.Free (PlayerCurrentAnim)
	end if
    elsif PlayerAction = "knife" then % Knife when using G and animate
	Sprite.Show (PlayerKnifeSprite)
	Sprite.Hide (PlayerWalkSprite)
	if PlayerFacing = "right" then
	    PlayerCurrentAnim := (PlayerKnifePics (NPCAnimTick div FrameSpeed mod PlayerKnifeFrames + 1))
	    Sprite.Animate (PlayerKnifeSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	elsif PlayerFacing = "left" then
	    PlayerCurrentAnim := Pic.Mirror (PlayerKnifePics (NPCAnimTick div FrameSpeed mod PlayerKnifeFrames + 1))
	    Sprite.Animate (PlayerKnifeSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	    Pic.Free (PlayerCurrentAnim)
	elsif PlayerFacing = "up" then
	    PlayerCurrentAnim := Pic.Rotate (PlayerKnifePics (NPCAnimTick div FrameSpeed mod PlayerKnifeFrames + 1), 90, 51, 51)
	    Sprite.Animate (PlayerKnifeSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	    Pic.Free (PlayerCurrentAnim)
	elsif PlayerFacing = "down" then
	    PlayerCurrentAnim := Pic.Rotate (PlayerKnifePics (NPCAnimTick div FrameSpeed mod PlayerKnifeFrames + 1), -90, 51, 51)
	    Sprite.Animate (PlayerKnifeSprite, PlayerCurrentAnim, PlayerX, PlayerY, false)
	    Pic.Free (PlayerCurrentAnim)
	end if
    end if

    % Events - Level: Tutorial
    if CurrentLevel = 0 and GameStart = 1 then
	% Guard (Bat, 1) killed event
	if PlayerAction = "knife" then
	    if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 then
		GuardBat1Dead := 1
	    end if
	end if

	% Wall Collision
	if PlayerY < 35 then
	    PlayerY := 34
	end if

	if PlayerX < 30 then
	    PlayerX := 31
	end if

	if PlayerX > 910 then
	    PlayerX := 909
	end if

	if not PlayerX > 330 then
	    if PlayerY > 668 then
		PlayerY := 667
	    end if
	end if

	if not PlayerX < 630 then
	    if PlayerY > 668 then
		PlayerY := 667
	    end if
	end if

	% Deaths
	if GuardBat1Dead = 1 then
	    Sprite.Hide (GuardBat1SwingSprite)
	    Sprite.Show (GuardBatDeadSprite)
	    GuardBat1X := 480
	    GuardBat1Y := 400
	    Sprite.Animate (GuardBatDeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat1X, GuardBat1Y, false)

	    MovementEnabled := 0
	end if

	% Guard (Bat, 1) Animation
	var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 90, 80, 80)
	Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, 430, 380, false)
	Pic.Free (GuardBatSwingAnim)

	% Tutorial Cutscene, waits for Enter key to be pressed above, then swaps the text.
	if Level00CutscenePlayed = 0 then
	    if PlayerY < 200 and MovementEnabled = 0 then
		PlayerY := PlayerY + 1
		Sprite.Show (GuardBat1SwingSprite)
	    else
		DialogueDone := 1
	    end if

	    if DialogueTick = 0 then
		Pic.Draw (Level00Bars, 0, 0, picCopy)
	    elsif DialogueTick = 1 then
		Pic.Draw (Level00Text1, 0, 0, picCopy)
	    elsif DialogueTick = 2 then
		Pic.Draw (Level00Text2, 0, 0, picCopy)
	    elsif DialogueTick = 3 then
		if GuardBat1Dead = 0 then
		    GuardBat1X := 430
		    GuardBat1Y := 380
		    MovementEnabled := 1
		end if
		Pic.Draw (Level00Text3, 0, 0, picCopy)
	    elsif DialogueTick = 4 then
		Pic.Draw (Level00Text4, 0, 0, picCopy)
	    elsif DialogueTick = 5 then
		Level00CutscenePlayed := 1
		DialogueDone := 0
	    end if
	else
	    MovementEnabled := 1
	    Pic.Draw (Level00, 0, 0, picCopy)

	    % Cleanup and Level Change to Level 1
	    if PlayerX > 350 and PlayerX < 660 and PlayerY > 768 then
		StartLevel01 ()
	    end if
	end if
    end if

    % Events - Level: 1
    if CurrentLevel = 1 then
	if Level01CutscenePlayed = 0 then
	    if PlayerY < 50 then
		PlayerY := PlayerY + 1
	    else
		MovementEnabled := 1
		Level01CutscenePlayed := 1
	    end if
	end if

	% Guard (Bat, 1) Alert events, movement, and animation.
	if PlayerX > 135 and GuardBat1Dead = 0 then
	    GuardBat1Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat1Alerted = 1 and PlayerDied = 0 and GuardBat1Dead = 0 then
	    if PlayerX > GuardBat1X then
		GuardBat1X := GuardBat1X + 2
		GuardBat1Direction := "right"
	    elsif GuardBat1Dead = 0 then
		GuardBat1X := GuardBat1X - 2
		GuardBat1Direction := "left"
	    end if

	    if PlayerY > GuardBat1Y then
		GuardBat1Y := GuardBat1Y + 1
	    elsif GuardBat1Dead = 0 then
		GuardBat1Y := GuardBat1Y - 1
	    end if

	    if GuardBat1Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat1Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
	    end if
	end if

	% Guard (Bat, 2) Alert events, movement, and animation.
	if PlayerX > 675 and PlayerY > 350 and GuardBat2Dead = 0 then
	    GuardBat2Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat2Alerted = 1 and PlayerDied = 0 and GuardBat2Dead = 0 then
	    if PlayerX > GuardBat2X then
		GuardBat2X := GuardBat2X + 2
		GuardBat2Direction := "right"
	    elsif GuardBat2Dead = 0 then
		GuardBat2X := GuardBat2X - 2
		GuardBat2Direction := "left"
	    end if

	    if PlayerY > GuardBat2Y then
		GuardBat2Y := GuardBat2Y + 1
	    elsif GuardBat2Dead = 0 then
		GuardBat2Y := GuardBat2Y - 1
	    end if

	    if GuardBat2Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat2Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
	    end if
	end if

	% Guard (Bat, 3) Alert events, movement, and animation.
	if PlayerX > 135 and GuardBat3Dead = 0 then
	    GuardBat3Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat3Alerted = 1 and PlayerDied = 0 and GuardBat3Dead = 0 then
	    if PlayerX > GuardBat3X then
		GuardBat3X := GuardBat3X + 2
		GuardBat3Direction := "right"
	    elsif GuardBat3Dead = 0 then
		GuardBat3X := GuardBat3X - 2
		GuardBat3Direction := "left"
	    end if

	    if PlayerY > GuardBat3Y then
		GuardBat3Y := GuardBat3Y + 1
	    elsif GuardBat3Dead = 0 then
		GuardBat3Y := GuardBat3Y - 1
	    end if

	    if GuardBat3Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat3Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
	    end if
	end if

	% Guard (Bat, 4) Alert events, movement, and animation.
	if PlayerX > 675 and PlayerY > 350 and GuardBat4Dead = 0 then
	    GuardBat4Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat4Alerted = 1 and PlayerDied = 0 and GuardBat4Dead = 0 then
	    if PlayerX > GuardBat4X then
		GuardBat4X := GuardBat4X + 2
		GuardBat4Direction := "right"
	    elsif GuardBat4Dead = 0 then
		GuardBat4X := GuardBat4X - 2
		GuardBat4Direction := "left"
	    end if

	    if PlayerY > GuardBat4Y then
		GuardBat4Y := GuardBat4Y + 1
	    elsif GuardBat4Dead = 0 then
		GuardBat4Y := GuardBat4Y - 1
	    end if

	    if GuardBat4Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat4Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
	    end if
	end if

	% Wall collision
	if PlayerX < 2 and PlayerY < 520 and not PlayerY > 600 then
	    PlayerX := 2
	end if

	if PlayerY > 325 and PlayerY < 346 and PlayerX < 675 then
	    PlayerY := 325
	end if

	if PlayerY > 325 and PlayerY < 346 and PlayerX > 755 then
	    PlayerY := 325
	end if

	if PlayerX > 964 then
	    PlayerX := 964
	end if

	if PlayerY > 708 then
	    PlayerY := 708
	end if

	if PlayerY < 0 and PlayerX > 135 then
	    PlayerY := 0
	end if

	if PlayerX < 2 and PlayerY > 600 then
	    PlayerX := 2
	end if

	% Weapon and Kill Events
	if PlayerAction = "knife" then
	    if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 then
		GuardBat1Dead := 1
	    end if

	    if PlayerX >= GuardBat2X and PlayerX <= GuardBat2X + 150 and PlayerY >= GuardBat2Y and PlayerY <= GuardBat2Y + 150 then
		GuardBat2Dead := 1
	    end if

	    if PlayerX >= GuardBat3X and PlayerX <= GuardBat3X + 150 and PlayerY >= GuardBat3Y and PlayerY <= GuardBat3Y + 150 then
		GuardBat3Dead := 1
	    end if

	    if PlayerX >= GuardBat4X and PlayerX <= GuardBat4X + 150 and PlayerY >= GuardBat4Y and PlayerY <= GuardBat4Y + 150 then
		GuardBat4Dead := 1
	    end if
	end if

	% Death Events
	if GuardBat1Dead = 1 then
	    Sprite.Hide (GuardBat1SwingSprite)
	    Sprite.Show (GuardBatDeadSprite)
	    Sprite.Animate (GuardBatDeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat1X, GuardBat1Y, false)
	end if

	if GuardBat2Dead = 1 then
	    Sprite.Hide (GuardBat2SwingSprite)
	    Sprite.Show (GuardBat2DeadSprite)
	    Sprite.Animate (GuardBat2DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat2X, GuardBat2Y, false)
	end if

	if GuardBat3Dead = 1 then
	    Sprite.Hide (GuardBat3SwingSprite)
	    Sprite.Show (GuardBat3DeadSprite)
	    Sprite.Animate (GuardBat3DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat3X, GuardBat3Y, false)
	end if

	if GuardBat4Dead = 1 then
	    Sprite.Hide (GuardBat4SwingSprite)
	    Sprite.Show (GuardBat4DeadSprite)
	    Sprite.Animate (GuardBat4DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat4X, GuardBat4Y, false)
	end if

	if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 and NPCAnimTick >= 130 and GuardBat1Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat2X and PlayerX <= GuardBat2X + 150 and PlayerY >= GuardBat2Y and PlayerY <= GuardBat2Y + 150 and NPCAnimTick >= 130 and GuardBat2Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat3X and PlayerX <= GuardBat3X + 150 and PlayerY >= GuardBat3Y and PlayerY <= GuardBat3Y + 150 and NPCAnimTick >= 130 and GuardBat3Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat4X and PlayerX <= GuardBat4X + 150 and PlayerY >= GuardBat4Y and PlayerY <= GuardBat4Y + 150 and NPCAnimTick >= 130 and GuardBat4Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerDied = 1 then
	    Sprite.Hide (PlayerKnifeSprite)
	    Sprite.Hide (PlayerWalkSprite)
	    Sprite.Show (PlayerDeadSprite)
	    Sprite.Animate (PlayerDeadSprite, (PlayerDeadPics (NPCAnimTick div FrameSpeed mod PlayerDeadFrames + 1)), PlayerX, PlayerY, false)
	    Pic.Draw (Level01Died, 0, 0, picCopy)
	else
	    Pic.Draw (Level01, 0, 0, picCopy)
	end if

	% Cleanup and Level Change to Level 2
	if PlayerX < -20 then
	    StartLevel02 ()
	end if
    end if

    % Events - Level: 2
    if CurrentLevel = 2 then
	if Level02CutscenePlayed = 0 then
	    if PlayerX > 950 then
		PlayerX := PlayerX - 1
	    else
		MovementEnabled := 1
		Level02CutscenePlayed := 1
	    end if
	end if

	% Guard (Bat, 1) Alert events, movement, and animation.
	if PlayerX < 850 and GuardBat1Dead = 0 then
	    GuardBat1Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat1Alerted = 1 and PlayerDied = 0 and GuardBat1Dead = 0 then
	    if PlayerX > GuardBat1X then
		GuardBat1X := GuardBat1X + 2
		GuardBat1Direction := "right"
	    elsif GuardBat1Dead = 0 then
		GuardBat1X := GuardBat1X - 2
		GuardBat1Direction := "left"
	    end if

	    if PlayerY > GuardBat1Y then
		GuardBat1Y := GuardBat1Y + 1
	    elsif GuardBat1Dead = 0 then
		GuardBat1Y := GuardBat1Y - 1
	    end if

	    if GuardBat1Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat1Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
	    end if
	end if

	% Guard (Bat, 2) Alert events, movement, and animation.
	if PlayerX < 850 and GuardBat2Dead = 0 then
	    GuardBat2Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat2Alerted = 1 and PlayerDied = 0 and GuardBat2Dead = 0 then
	    if PlayerX > GuardBat2X then
		GuardBat2X := GuardBat2X + 2
		GuardBat2Direction := "right"
	    elsif GuardBat2Dead = 0 then
		GuardBat2X := GuardBat2X - 2
		GuardBat2Direction := "left"
	    end if

	    if PlayerY > GuardBat2Y then
		GuardBat2Y := GuardBat2Y + 1
	    elsif GuardBat2Dead = 0 then
		GuardBat2Y := GuardBat2Y - 1
	    end if

	    if GuardBat2Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat2Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
	    end if
	end if

	% Guard (Bat, 3) Alert events, movement, and animation.
	if PlayerX < 850 and GuardBat3Dead = 0 then
	    GuardBat3Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat3Alerted = 1 and PlayerDied = 0 and GuardBat3Dead = 0 then
	    if PlayerX > GuardBat3X then
		GuardBat3X := GuardBat3X + 2
		GuardBat3Direction := "right"
	    elsif GuardBat3Dead = 0 then
		GuardBat3X := GuardBat3X - 2
		GuardBat3Direction := "left"
	    end if

	    if PlayerY > GuardBat3Y then
		GuardBat3Y := GuardBat3Y + 1
	    elsif GuardBat3Dead = 0 then
		GuardBat3Y := GuardBat3Y - 1
	    end if

	    if GuardBat3Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat3Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
	    end if
	end if

	% Guard (Bat, 4) Alert events, movement, and animation.
	if PlayerX < 850 and GuardBat4Dead = 0 then
	    GuardBat4Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat4Alerted = 1 and PlayerDied = 0 and GuardBat4Dead = 0 then
	    if PlayerX > GuardBat4X then
		GuardBat4X := GuardBat4X + 2
		GuardBat4Direction := "right"
	    elsif GuardBat4Dead = 0 then
		GuardBat4X := GuardBat4X - 2
		GuardBat4Direction := "left"
	    end if

	    if PlayerY > GuardBat4Y then
		GuardBat4Y := GuardBat4Y + 1
	    elsif GuardBat4Dead = 0 then
		GuardBat4Y := GuardBat4Y - 1
	    end if

	    if GuardBat4Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat4Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
	    end if
	end if

	% Guard (Bat, 5) Alert events, movement, and animation.
	if PlayerX < 850 and GuardBat5Dead = 0 then
	    GuardBat5Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat5SwingSprite, GuardBatSwingAnim, GuardBat5X, GuardBat5Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat5Alerted = 1 and PlayerDied = 0 and GuardBat5Dead = 0 then
	    if PlayerX > GuardBat5X then
		GuardBat5X := GuardBat5X + 2
		GuardBat5Direction := "right"
	    elsif GuardBat5Dead = 0 then
		GuardBat5X := GuardBat5X - 2
		GuardBat5Direction := "left"
	    end if

	    if PlayerY > GuardBat5Y then
		GuardBat5Y := GuardBat5Y + 1
	    elsif GuardBat5Dead = 0 then
		GuardBat5Y := GuardBat5Y - 1
	    end if

	    if GuardBat5Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat5SwingSprite, GuardBatSwingAnim, GuardBat5X, GuardBat5Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat5Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat5SwingSprite, GuardBatSwingAnim, GuardBat5X, GuardBat5Y, false)
	    end if
	end if

	% Weapon and Kill Events
	if PlayerAction = "knife" then
	    if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 then
		GuardBat1Dead := 1
	    end if

	    if PlayerX >= GuardBat2X and PlayerX <= GuardBat2X + 150 and PlayerY >= GuardBat2Y and PlayerY <= GuardBat2Y + 150 then
		GuardBat2Dead := 1
	    end if

	    if PlayerX >= GuardBat3X and PlayerX <= GuardBat3X + 150 and PlayerY >= GuardBat3Y and PlayerY <= GuardBat3Y + 150 then
		GuardBat3Dead := 1
	    end if

	    if PlayerX >= GuardBat4X and PlayerX <= GuardBat4X + 150 and PlayerY >= GuardBat4Y and PlayerY <= GuardBat4Y + 150 then
		GuardBat4Dead := 1
	    end if

	    if PlayerX >= GuardBat5X and PlayerX <= GuardBat5X + 150 and PlayerY >= GuardBat5Y and PlayerY <= GuardBat5Y + 150 then
		GuardBat5Dead := 1
	    end if
	end if

	% Death Events
	if GuardBat1Dead = 1 then
	    Sprite.Hide (GuardBat1SwingSprite)
	    Sprite.Show (GuardBatDeadSprite)
	    Sprite.Animate (GuardBatDeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat1X, GuardBat1Y, false)
	end if

	if GuardBat2Dead = 1 then
	    Sprite.Hide (GuardBat2SwingSprite)
	    Sprite.Show (GuardBat2DeadSprite)
	    Sprite.Animate (GuardBat2DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat2X, GuardBat2Y, false)
	end if

	if GuardBat3Dead = 1 then
	    Sprite.Hide (GuardBat3SwingSprite)
	    Sprite.Show (GuardBat3DeadSprite)
	    Sprite.Animate (GuardBat3DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat3X, GuardBat3Y, false)
	end if

	if GuardBat4Dead = 1 then
	    Sprite.Hide (GuardBat4SwingSprite)
	    Sprite.Show (GuardBat4DeadSprite)
	    Sprite.Animate (GuardBat4DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat4X, GuardBat4Y, false)
	end if

	if GuardBat5Dead = 1 then
	    Sprite.Hide (GuardBat5SwingSprite)
	    Sprite.Show (GuardBat5DeadSprite)
	    Sprite.Animate (GuardBat5DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat5X, GuardBat5Y, false)
	end if

	if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 and NPCAnimTick >= 130 and GuardBat1Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat2X and PlayerX <= GuardBat2X + 150 and PlayerY >= GuardBat2Y and PlayerY <= GuardBat2Y + 150 and NPCAnimTick >= 130 and GuardBat2Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat3X and PlayerX <= GuardBat3X + 150 and PlayerY >= GuardBat3Y and PlayerY <= GuardBat3Y + 150 and NPCAnimTick >= 130 and GuardBat3Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat4X and PlayerX <= GuardBat4X + 150 and PlayerY >= GuardBat4Y and PlayerY <= GuardBat4Y + 150 and NPCAnimTick >= 130 and GuardBat4Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat5X and PlayerX <= GuardBat5X + 150 and PlayerY >= GuardBat5Y and PlayerY <= GuardBat5Y + 150 and NPCAnimTick >= 130 and GuardBat5Dead = 0 then
	    PlayerDied := 1
	end if

	% Wall collision
	if Level02CutscenePlayed = 1 then
	    if PlayerX > 950 then
		PlayerX := 950
	    end if

	    if PlayerX < 0 then
		PlayerX := 0
	    end if

	    if PlayerY < 0 then
		PlayerY := 0
	    end if

	    if PlayerY > 700 then
		if PlayerX < 200 or PlayerX > 490 then
		    PlayerY := 700
		end if
	    end if
	end if

	if PlayerDied = 1 then
	    Sprite.Hide (PlayerKnifeSprite)
	    Sprite.Hide (PlayerWalkSprite)
	    Sprite.Show (PlayerDeadSprite)
	    Sprite.Animate (PlayerDeadSprite, (PlayerDeadPics (NPCAnimTick div FrameSpeed mod PlayerDeadFrames + 1)), PlayerX, PlayerY, false)
	    Pic.Draw (Level02Died, 0, 0, picCopy)
	elsif PlayerDied = 0 then
	    Pic.Draw (Level02, 0, 0, picCopy)
	end if

	if PlayerX > 200 and PlayerX < 490 and PlayerY > 768 then
	    StartLevel03 ()
	end if
    end if

    % Events - Level: 3
    if CurrentLevel = 3 then
	if Level03CutscenePlayed = 0 then
	    if PlayerY < 30 then
		PlayerY := PlayerY + 3
	    else
		DialogueDone := 1
	    end if

	    if DialogueTick = 0 then
		Pic.Draw (Level03Text1, 0, 0, picCopy)
	    elsif DialogueTick = 1 then
		Pic.Draw (Level03Text2, 0, 0, picCopy)
	    elsif DialogueTick = 2 then
		Pic.Draw (Level03Text3, 0, 0, picCopy)
	    elsif DialogueTick = 3 then
		MovementEnabled := 1
		Level03CutscenePlayed := 1
		DialogueDone := 0
	    end if
	else
	    Pic.Draw (Level03, 0, 0, picCopy)
	end if

	if GuardBat1Dead = 1 and GuardBat2Dead = 1 and GuardBat3Dead = 1 and GuardBat4Dead = 1 and GuardBat5Dead = 1 and Level03CutscenePlayed = 1 then
	    DialogueDone := 1
	    MovementEnabled := 0

	    if DialogueTick = 3 then
		Pic.Draw (Level03Text4, 0, 0, picCopy)
	    elsif DialogueTick = 4 then
		Pic.Draw (Level03Text5, 0, 0, picCopy)
	    elsif DialogueTick = 5 then
		Pic.Draw (Level03Text6, 0, 0, picCopy)
	    elsif DialogueTick = 6 then
		EndGame := 1
		DialogueDone := 0
	    end if
	end if

	if EndGame = 1 then
	    Sprite.Hide (GuardBatDeadSprite)
	    Sprite.Hide (GuardBat2DeadSprite)
	    Sprite.Hide (GuardBat3DeadSprite)
	    Sprite.Hide (GuardBat4DeadSprite)
	    Sprite.Hide (GuardBat5DeadSprite)
	    Sprite.Hide (PlayerDeadSprite)
	    Sprite.Hide (PlayerWalkSprite)
	    Sprite.Hide (PlayerKnifeSprite)
	    Pic.Draw (Level03Text7, 0, 0, picCopy)
	end if

	% Guard (Bat, 1) Alert events, movement, and animation.
	if Level03CutscenePlayed = 1 and GuardBat1Dead = 0 then
	    GuardBat1Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat1Alerted = 1 and PlayerDied = 0 and GuardBat1Dead = 0 then
	    if PlayerX > GuardBat1X then
		GuardBat1X := GuardBat1X + 2
		GuardBat1Direction := "right"
	    elsif GuardBat1Dead = 0 then
		GuardBat1X := GuardBat1X - 2
		GuardBat1Direction := "left"
	    end if

	    if PlayerY > GuardBat1Y then
		GuardBat1Y := GuardBat1Y + 1
	    elsif GuardBat1Dead = 0 then
		GuardBat1Y := GuardBat1Y - 1
	    end if

	    if GuardBat1Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat1Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat1SwingSprite, GuardBatSwingAnim, GuardBat1X, GuardBat1Y, false)
	    end if
	end if

	% Guard (Bat, 2) Alert events, movement, and animation.
	if Level03CutscenePlayed = 1 and GuardBat2Dead = 0 then
	    GuardBat2Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat2Alerted = 1 and PlayerDied = 0 and GuardBat2Dead = 0 then
	    if PlayerX > GuardBat2X then
		GuardBat2X := GuardBat2X + 2
		GuardBat2Direction := "right"
	    elsif GuardBat2Dead = 0 then
		GuardBat2X := GuardBat2X - 2
		GuardBat2Direction := "left"
	    end if

	    if PlayerY > GuardBat2Y then
		GuardBat2Y := GuardBat2Y + 1
	    elsif GuardBat2Dead = 0 then
		GuardBat2Y := GuardBat2Y - 1
	    end if

	    if GuardBat2Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat2Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat2SwingSprite, GuardBatSwingAnim, GuardBat2X, GuardBat2Y, false)
	    end if
	end if

	% Guard (Bat, 3) Alert events, movement, and animation.
	if Level03CutscenePlayed = 1 and GuardBat3Dead = 0 then
	    GuardBat3Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat3Alerted = 1 and PlayerDied = 0 and GuardBat3Dead = 0 then
	    if PlayerX > GuardBat3X then
		GuardBat3X := GuardBat3X + 2
		GuardBat3Direction := "right"
	    elsif GuardBat3Dead = 0 then
		GuardBat3X := GuardBat3X - 2
		GuardBat3Direction := "left"
	    end if

	    if PlayerY > GuardBat3Y then
		GuardBat3Y := GuardBat3Y + 1
	    elsif GuardBat3Dead = 0 then
		GuardBat3Y := GuardBat3Y - 1
	    end if

	    if GuardBat3Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat3Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat3SwingSprite, GuardBatSwingAnim, GuardBat3X, GuardBat3Y, false)
	    end if
	end if

	% Guard (Bat, 4) Alert events, movement, and animation.
	if Level03CutscenePlayed = 1 and GuardBat4Dead = 0 then
	    GuardBat4Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat4Alerted = 1 and PlayerDied = 0 and GuardBat4Dead = 0 then
	    if PlayerX > GuardBat4X then
		GuardBat4X := GuardBat4X + 2
		GuardBat4Direction := "right"
	    elsif GuardBat4Dead = 0 then
		GuardBat4X := GuardBat4X - 2
		GuardBat4Direction := "left"
	    end if

	    if PlayerY > GuardBat4Y then
		GuardBat4Y := GuardBat4Y + 1
	    elsif GuardBat4Dead = 0 then
		GuardBat4Y := GuardBat4Y - 1
	    end if

	    if GuardBat4Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat4Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat4SwingSprite, GuardBatSwingAnim, GuardBat4X, GuardBat4Y, false)
	    end if
	end if

	% Guard (Bat, 5) Alert events, movement, and animation.
	if Level03CutscenePlayed = 1 and GuardBat5Dead = 0 then
	    GuardBat5Alerted := 1
	else
	    var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (1), 180, 80, 80)
	    Sprite.Animate (GuardBat5SwingSprite, GuardBatSwingAnim, GuardBat5X, GuardBat5Y, false)
	    Pic.Free (GuardBatSwingAnim)
	end if

	if GuardBat5Alerted = 1 and PlayerDied = 0 and GuardBat5Dead = 0 then
	    if PlayerX > GuardBat5X then
		GuardBat5X := GuardBat5X + 2
		GuardBat5Direction := "right"
	    elsif GuardBat5Dead = 0 then
		GuardBat5X := GuardBat5X - 2
		GuardBat5Direction := "left"
	    end if

	    if PlayerY > GuardBat5Y then
		GuardBat5Y := GuardBat5Y + 1
	    elsif GuardBat5Dead = 0 then
		GuardBat5Y := GuardBat5Y - 1
	    end if

	    if GuardBat5Direction = "left" then
		var GuardBatSwingAnim : int := Pic.Rotate (GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1), 180, 80, 80)
		Sprite.Animate (GuardBat5SwingSprite, GuardBatSwingAnim, GuardBat5X, GuardBat5Y, false)
		Pic.Free (GuardBatSwingAnim)
	    elsif GuardBat5Direction = "right" then
		var GuardBatSwingAnim : int := GuardBat1SwingPics (NPCAnimTick div FrameSpeed mod GuardBat1SwingFrames + 1)
		Sprite.Animate (GuardBat5SwingSprite, GuardBatSwingAnim, GuardBat5X, GuardBat5Y, false)
	    end if
	end if

	% Weapon and Kill Events
	if PlayerAction = "knife" then
	    if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 then
		GuardBat1Dead := 1
	    end if

	    if PlayerX >= GuardBat2X and PlayerX <= GuardBat2X + 150 and PlayerY >= GuardBat2Y and PlayerY <= GuardBat2Y + 150 then
		GuardBat2Dead := 1
	    end if

	    if PlayerX >= GuardBat3X and PlayerX <= GuardBat3X + 150 and PlayerY >= GuardBat3Y and PlayerY <= GuardBat3Y + 150 then
		GuardBat3Dead := 1
	    end if

	    if PlayerX >= GuardBat4X and PlayerX <= GuardBat4X + 150 and PlayerY >= GuardBat4Y and PlayerY <= GuardBat4Y + 150 then
		GuardBat4Dead := 1
	    end if

	    if PlayerX >= GuardBat5X and PlayerX <= GuardBat5X + 150 and PlayerY >= GuardBat5Y and PlayerY <= GuardBat5Y + 150 then
		GuardBat5Dead := 1
	    end if
	end if

	% Death Events
	if GuardBat1Dead = 1 and EndGame = 0 then
	    Sprite.Hide (GuardBat1SwingSprite)
	    Sprite.Show (GuardBatDeadSprite)
	    Sprite.Animate (GuardBatDeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat1X, GuardBat1Y, false)
	end if

	if GuardBat2Dead = 1 and EndGame = 0 then
	    Sprite.Hide (GuardBat2SwingSprite)
	    Sprite.Show (GuardBat2DeadSprite)
	    Sprite.Animate (GuardBat2DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat2X, GuardBat2Y, false)
	end if

	if GuardBat3Dead = 1 and EndGame = 0 then
	    Sprite.Hide (GuardBat3SwingSprite)
	    Sprite.Show (GuardBat3DeadSprite)
	    Sprite.Animate (GuardBat3DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat3X, GuardBat3Y, false)
	end if

	if GuardBat4Dead = 1 and EndGame = 0 then
	    Sprite.Hide (GuardBat4SwingSprite)
	    Sprite.Show (GuardBat4DeadSprite)
	    Sprite.Animate (GuardBat4DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat4X, GuardBat4Y, false)
	end if

	if GuardBat5Dead = 1 and EndGame = 0 then
	    Sprite.Hide (GuardBat5SwingSprite)
	    Sprite.Show (GuardBat5DeadSprite)
	    Sprite.Animate (GuardBat5DeadSprite, (GuardBatDeadPics (NPCAnimTick div FrameSpeed mod GuardBatDeadFrames + 1)), GuardBat5X, GuardBat5Y, false)
	end if

	if PlayerX >= GuardBat1X and PlayerX <= GuardBat1X + 150 and PlayerY >= GuardBat1Y and PlayerY <= GuardBat1Y + 150 and NPCAnimTick >= 130 and GuardBat1Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat2X and PlayerX <= GuardBat2X + 150 and PlayerY >= GuardBat2Y and PlayerY <= GuardBat2Y + 150 and NPCAnimTick >= 130 and GuardBat2Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat3X and PlayerX <= GuardBat3X + 150 and PlayerY >= GuardBat3Y and PlayerY <= GuardBat3Y + 150 and NPCAnimTick >= 130 and GuardBat3Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat4X and PlayerX <= GuardBat4X + 150 and PlayerY >= GuardBat4Y and PlayerY <= GuardBat4Y + 150 and NPCAnimTick >= 130 and GuardBat4Dead = 0 then
	    PlayerDied := 1
	end if

	if PlayerX >= GuardBat5X and PlayerX <= GuardBat5X + 150 and PlayerY >= GuardBat5Y and PlayerY <= GuardBat5Y + 150 and NPCAnimTick >= 130 and GuardBat5Dead = 0 then
	    PlayerDied := 1
	end if

	if Level03CutscenePlayed = 1 then
	    if PlayerX > 950 then
		PlayerX := 950
	    end if

	    if PlayerX < 0 then
		PlayerX := 0
	    end if

	    if PlayerY < 0 then
		PlayerY := 0
	    end if

	    if PlayerY > 700 then
		PlayerY := 700
	    end if
	end if

	if PlayerDied = 1 then
	    Sprite.Hide (PlayerKnifeSprite)
	    Sprite.Hide (PlayerWalkSprite)
	    Sprite.Show (PlayerDeadSprite)
	    Sprite.Animate (PlayerDeadSprite, (PlayerDeadPics (NPCAnimTick div FrameSpeed mod PlayerDeadFrames + 1)), PlayerX, PlayerY, false)
	    Pic.Draw (Level03Died, 0, 0, picCopy)
	end if
    end if
    delay (17)
    View.Update
end loop
