//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

// Nagios
const NAGIOS_PORT = 8008;

const USE_HEARTBEAT = true;

// Mail
const USE_SMTP           = true;
const MAIL_SERVER        = "192.168.203.5";
const MAIL_ADDRESS       = "InvestorRelations@autostadt.de";
const MAIL_ADDRESS_NO_REPLY = "no-reply-InvestorRelations@autostadt.de";
const MAIL_NAME_NO_REPLY = "Boersenspiel-Mailer";

const HIGHSCORES_COUNT = 100;

// Smallest zoom factor.
const MINZOOMFACTOR     = 0.001;

// Enable/disable the application itself (for testing).
const APPLICATION = true;

// Enable/disable various bodies used for testing.
const TEST_BACKGROUND   = false;
const TEST_NEWSPAPER    = false;
const TEST_TIMERWIDGET  = false;
const TEST_CHART        = false;
const TEST_KEYBOARD     = false;
const TEST_TEXTBOX      = false;
const TEST_TEXTBUTTON   = false;

// Root directory for game content.
const CONTENT = "CONTENT";

// Layer manager / layout.
const LAYOUT_CONFIG = "CONFIG/layers.xml";
const LAYER_PATH    = "Layers/";

// Game logic and charts.
const GAME_CONFIG = "CONFIG/market.xml";
const MARKET_OFFSET = 5.0;
const MAX_PRICE_HISTORY_STACK = 50;
const CHART_UPDATES_PER_SECOND = 1.5;
const CHART_VALUE_OFFSET = 7;
var   GAME_TIME = 3.0*60.0; // sec
const GAME_TIME_WARNING = 10.0; // sec
const BOUNDS_BORDER = 0.4;
const GRID_TEX = CONTENT+"/grid.png";
const EVENT_INFLUENCE = 0.35;
const EVENT_INTERVAL = 15; // seconds
const EVENT_HISTORY = 25;

const INITIAL_CREDIT = 1000;
const LOWER_CREDIT = 980;
const MID_CREDIT   = 1020;
const UPPER_CREDIT = 1250;

// Duration of key highlight.
//   for TextButton
const BUTTON_HIGHLIGHT_DURATION   = 200.0; //msec
//   for Keyboard
const KEYBOARD_HIGHLIGHT_DURATION = 100.0; //msec

// timeout for workspace and lottery
const WORKSPACE_TIMEOUT = 1*60*1000; //msec

// Common values for animations.
const STANDARD_FADE_TIME = 500.0; //msec

// Workspace animations.
const JOIN_FADEIN_TIME       = STANDARD_FADE_TIME; //msec
const JOIN_FADEOUT_TIME      = STANDARD_FADE_TIME; //msec
const INTRO_FADEIN_TIME      = STANDARD_FADE_TIME; //msec
const INTRO_FADEOUT_TIME     = STANDARD_FADE_TIME; //msec
const PLAY_FADEIN_TIME       = STANDARD_FADE_TIME; //msec
const PLAY_FADEOUT_TIME      = STANDARD_FADE_TIME; //msec
const OUTRO_FADEIN_TIME      = STANDARD_FADE_TIME; //msec
const OUTRO_ASK_FADEIN_TIME  = STANDARD_FADE_TIME; //msec
const OUTRO_ASK_FADEOUT_TIME = STANDARD_FADE_TIME; //msec
const BACKGROUND_FADEIN_TIME  = STANDARD_FADE_TIME; //msec
const BACKGROUND_FADEOUT_TIME = STANDARD_FADE_TIME; //msec

// Lottery animations.
const LOTTERY_BACKGROUND_FADEIN_TIME  = STANDARD_FADE_TIME; //msec
const LOTTERY_NAME_FADEIN_TIME        = STANDARD_FADE_TIME; //msec
const LOTTERY_NAME_FADEOUT_TIME       = STANDARD_FADE_TIME; //msec
const LOTTERY_EMAIL_FADEIN_TIME       = STANDARD_FADE_TIME; //msec
const LOTTERY_EMAIL_FADEOUT_TIME      = STANDARD_FADE_TIME; //msec
const LOTTERY_PHONE_FADEIN_TIME       = STANDARD_FADE_TIME; //msec
const LOTTERY_PHONE_FADEOUT_TIME      = STANDARD_FADE_TIME; //msec
const LOTTERY_VERIFY_FADEIN_TIME      = STANDARD_FADE_TIME; //msec
const LOTTERY_VERIFY_FADEOUT_TIME     = STANDARD_FADE_TIME; //msec
const LOTTERY_THANKS_FADEIN_TIME      = STANDARD_FADE_TIME; //msec
const LOTTERY_THANKS_FADEOUT_TIME     = STANDARD_FADE_TIME; //msec
const LOTTERY_BACKGROUND_FADEOUT_TIME = STANDARD_FADE_TIME; //msec

// Scroller.
var INTRO_SCROLLER_INITIAL_PAUSE = 4000.0; //msec
var INTRO_SCROLLER_FINAL_PAUSE   = 4000.0; //msec
var OUTRO_SCROLLER_INITIAL_PAUSE = 4000.0; //msec
var OUTRO_SCROLLER_FINAL_PAUSE   = 4000.0; //msec
var OUTRO_SCROLLER_LEAVE_SPACE   =     55; //pixels

var   SCROLLER_SPEED     = 11.0;   //pixels/sec
const SCROLLER_PRESCROLL = -5;   //pixels
const SCROLLER_SPACE     = 90;     //pixels

// Rotation speed for the STC, given in seconds per rotation.
const STOCKCHARTCIRCUS_TIME_FULLCIRCLE  = 90000.0; //msec

// Newspaper configuration.
//   data files
const NEWSPAPER_LAYOUTS      = CONTENT + "/newspaper_layouts.xml";
const NEWSPAPER_STYLES       = CONTENT + "/newspaper_styles.xml";
const NEWSPAPER_IMAGES       = CONTENT + "/newspaper_images";
//   final scale of the newspaper
const NEWSPAPER_SCALE        = 0.7;
//   fade-in and fade-out animation durations
const NEWSPAPER_FADEIN_TIME  = 1000.0; //msec
const NEWSPAPER_FADEOUT_TIME = 600.0; //msec
//   display duration
const NEWSPAPER_ACTIVE_DURATION = 10.0; //sec
//   grace time between touches
const NEWSPAPER_FADEOUT_GRACE   = 1.5; //sec
//   amount to rotate on fade-in
const NEWSPAPER_ROTATION     = 180.0;
//   specification of drop zone for fade-in
const NEWSPAPER_DROP_X       = 700-75;
const NEWSPAPER_DROP_Y       = 525-75;
const NEWSPAPER_DROP_WIDTH   = 150;
const NEWSPAPER_DROP_HEIGHT  = 150;
//   if true, newspaper dumps all its images on startup
const NEWSPAPER_DUMP_IMAGES = false;

var ourEventSoundVolumes = {};
    ourEventSoundVolumes["SOUNDS/Nachricht_faellt_2.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/06-Nachricht verschwindet.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/05-Nachricht bewegen 1.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/05-Nachricht bewegen 2.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/05-Nachricht bewegen 3.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/05-Nachricht bewegen 4.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/04-Artikel 2.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/04-Fax 2.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/04-Fax 3.aif"] = 0.6;
    ourEventSoundVolumes["SOUNDS/04-Telex.aif"] = 1.0;
    ourEventSoundVolumes["SOUNDS/04-Geruecht.aif"] = 1.0;


// XXX: pointer timeout hack
const MAX_POINTER_LIFETIME = 15000; //msec


const BUTTON_DISABLED_ALPHA = 0.4;
