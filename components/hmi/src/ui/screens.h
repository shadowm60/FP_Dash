#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_DASH = 1,
    SCREEN_ID_RACE = 2,
    SCREEN_ID_ENGINE = 3,
    SCREEN_ID_EX_TACHO = 4,
    SCREEN_ID_EX_NEEDLE = 5,
    SCREEN_ID_EX_BARS = 6,
    SCREEN_ID_EX_GAUGES = 7,
    SCREEN_ID_EX_STRIP = 8,
    SCREEN_ID_SETTINGS = 9,
    _SCREEN_ID_LAST = 9
};

typedef struct _objects_t {
    lv_obj_t *dash;
    lv_obj_t *race;
    lv_obj_t *engine;
    lv_obj_t *ex_tacho;
    lv_obj_t *ex_needle;
    lv_obj_t *ex_bars;
    lv_obj_t *ex_gauges;
    lv_obj_t *ex_strip;
    lv_obj_t *settings;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *obj2;
    lv_obj_t *rpm_arc;
    lv_obj_t *obj3;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *obj17;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *obj21;
    lv_obj_t *obj22;
    lv_obj_t *obj23;
    lv_obj_t *obj24;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *obj34;
    lv_obj_t *obj35;
    lv_obj_t *obj36;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *obj39;
    lv_obj_t *obj40;
    lv_obj_t *obj41;
    lv_obj_t *obj42;
    lv_obj_t *obj43;
    lv_obj_t *obj44;
    lv_obj_t *obj45;
    lv_obj_t *obj46;
    lv_obj_t *obj47;
    lv_obj_t *obj48;
    lv_obj_t *obj49;
    lv_obj_t *obj50;
    lv_obj_t *obj51;
    lv_obj_t *obj52;
    lv_obj_t *obj53;
    lv_obj_t *obj54;
    lv_obj_t *obj55;
    lv_obj_t *obj56;
    lv_obj_t *obj57;
    lv_obj_t *obj58;
    lv_obj_t *obj59;
    lv_obj_t *obj60;
    lv_obj_t *obj61;
    lv_obj_t *obj62;
    lv_obj_t *obj63;
    lv_obj_t *obj64;
    lv_obj_t *obj65;
    lv_obj_t *obj66;
    lv_obj_t *obj67;
    lv_obj_t *obj68;
    lv_obj_t *obj69;
    lv_obj_t *obj70;
    lv_obj_t *obj71;
    lv_obj_t *obj72;
    lv_obj_t *obj73;
    lv_obj_t *obj74;
    lv_obj_t *obj75;
    lv_obj_t *obj76;
    lv_obj_t *obj77;
    lv_obj_t *obj78;
    lv_obj_t *obj79;
    lv_obj_t *obj80;
    lv_obj_t *obj81;
    lv_obj_t *obj82;
    lv_obj_t *obj83;
    lv_obj_t *obj84;
    lv_obj_t *obj85;
    lv_obj_t *obj86;
    lv_obj_t *obj87;
    lv_obj_t *obj88;
    lv_obj_t *obj89;
    lv_obj_t *obj90;
    lv_obj_t *obj91;
    lv_obj_t *obj92;
    lv_obj_t *obj93;
    lv_obj_t *obj94;
    lv_obj_t *obj95;
    lv_obj_t *obj96;
    lv_obj_t *obj97;
    lv_obj_t *obj98;
    lv_obj_t *obj99;
    lv_obj_t *obj100;
    lv_obj_t *obj101;
    lv_obj_t *obj102;
    lv_obj_t *obj103;
    lv_obj_t *obj104;
    lv_obj_t *obj105;
    lv_obj_t *obj106;
    lv_obj_t *obj107;
    lv_obj_t *obj108;
    lv_obj_t *obj109;
    lv_obj_t *obj110;
    lv_obj_t *obj111;
    lv_obj_t *obj112;
    lv_obj_t *obj113;
    lv_obj_t *obj114;
    lv_obj_t *obj115;
    lv_obj_t *obj116;
    lv_obj_t *obj117;
    lv_obj_t *obj118;
    lv_obj_t *obj119;
    lv_obj_t *obj120;
    lv_obj_t *obj121;
    lv_obj_t *obj122;
    lv_obj_t *obj123;
    lv_obj_t *obj124;
    lv_obj_t *obj125;
    lv_obj_t *obj126;
    lv_obj_t *obj127;
    lv_obj_t *obj128;
    lv_obj_t *obj129;
    lv_obj_t *obj130;
    lv_obj_t *obj131;
    lv_obj_t *obj132;
    lv_obj_t *obj133;
    lv_obj_t *obj134;
    lv_obj_t *obj135;
    lv_obj_t *obj136;
    lv_obj_t *obj137;
    lv_obj_t *obj138;
    lv_obj_t *obj139;
    lv_obj_t *obj140;
    lv_obj_t *obj141;
    lv_obj_t *obj142;
    lv_obj_t *obj143;
    lv_obj_t *obj144;
    lv_obj_t *obj145;
    lv_obj_t *obj146;
    lv_obj_t *obj147;
    lv_obj_t *obj148;
    lv_obj_t *obj149;
    lv_obj_t *obj150;
    lv_obj_t *obj151;
    lv_obj_t *obj152;
    lv_obj_t *obj153;
    lv_obj_t *obj154;
    lv_obj_t *obj155;
    lv_obj_t *obj156;
    lv_obj_t *obj157;
    lv_obj_t *obj158;
    lv_obj_t *obj159;
    lv_obj_t *obj160;
    lv_obj_t *obj161;
    lv_obj_t *obj162;
    lv_obj_t *obj163;
    lv_obj_t *obj164;
    lv_obj_t *obj165;
    lv_obj_t *obj166;
    lv_obj_t *obj167;
    lv_obj_t *obj168;
    lv_obj_t *obj169;
    lv_obj_t *obj170;
    lv_obj_t *obj171;
    lv_obj_t *obj172;
    lv_obj_t *obj173;
    lv_obj_t *obj174;
    lv_obj_t *obj175;
    lv_obj_t *obj176;
    lv_obj_t *obj177;
    lv_obj_t *obj178;
    lv_obj_t *obj179;
    lv_obj_t *obj180;
    lv_obj_t *obj181;
    lv_obj_t *obj182;
    lv_obj_t *obj183;
    lv_obj_t *obj184;
    lv_obj_t *obj185;
    lv_obj_t *obj186;
    lv_obj_t *obj187;
    lv_obj_t *obj188;
    lv_obj_t *obj189;
    lv_obj_t *obj190;
    lv_obj_t *obj191;
    lv_obj_t *obj192;
    lv_obj_t *obj193;
    lv_obj_t *obj194;
    lv_obj_t *obj195;
    lv_obj_t *obj196;
    lv_obj_t *obj197;
    lv_obj_t *obj198;
    lv_obj_t *obj199;
    lv_obj_t *obj200;
    lv_obj_t *obj201;
    lv_obj_t *obj202;
    lv_obj_t *obj203;
    lv_obj_t *obj204;
    lv_obj_t *obj205;
    lv_obj_t *obj206;
    lv_obj_t *obj207;
    lv_obj_t *obj208;
    lv_obj_t *obj209;
    lv_obj_t *obj210;
    lv_obj_t *obj211;
    lv_obj_t *obj212;
    lv_obj_t *obj213;
    lv_obj_t *obj214;
    lv_obj_t *obj215;
    lv_obj_t *obj216;
    lv_obj_t *obj217;
    lv_obj_t *obj218;
    lv_obj_t *obj219;
    lv_obj_t *obj220;
    lv_obj_t *obj221;
    lv_obj_t *obj222;
    lv_obj_t *obj223;
    lv_obj_t *obj224;
    lv_obj_t *obj225;
    lv_obj_t *obj226;
    lv_obj_t *obj227;
    lv_obj_t *obj228;
    lv_obj_t *obj229;
    lv_obj_t *obj230;
    lv_obj_t *obj231;
    lv_obj_t *obj232;
    lv_obj_t *obj233;
    lv_obj_t *obj234;
    lv_obj_t *obj235;
    lv_obj_t *obj236;
    lv_obj_t *obj237;
    lv_obj_t *obj238;
    lv_obj_t *obj239;
    lv_obj_t *obj240;
    lv_obj_t *obj241;
    lv_obj_t *obj242;
    lv_obj_t *obj243;
    lv_obj_t *obj244;
    lv_obj_t *obj245;
    lv_obj_t *obj246;
    lv_obj_t *obj247;
    lv_obj_t *obj248;
    lv_obj_t *obj249;
    lv_obj_t *obj250;
    lv_obj_t *obj251;
    lv_obj_t *obj252;
    lv_obj_t *obj253;
    lv_obj_t *obj254;
    lv_obj_t *obj255;
    lv_obj_t *obj256;
    lv_obj_t *obj257;
    lv_obj_t *obj258;
    lv_obj_t *obj259;
    lv_obj_t *obj260;
    lv_obj_t *obj261;
    lv_obj_t *obj262;
    lv_obj_t *obj263;
    lv_obj_t *obj264;
    lv_obj_t *obj265;
    lv_obj_t *obj266;
    lv_obj_t *obj267;
    lv_obj_t *obj268;
    lv_obj_t *obj269;
    lv_obj_t *obj270;
    lv_obj_t *obj271;
    lv_obj_t *obj272;
    lv_obj_t *obj273;
    lv_obj_t *obj274;
    lv_obj_t *obj275;
    lv_obj_t *obj276;
    lv_obj_t *obj277;
    lv_obj_t *obj278;
    lv_obj_t *obj279;
    lv_obj_t *obj280;
    lv_obj_t *obj281;
    lv_obj_t *obj282;
    lv_obj_t *obj283;
    lv_obj_t *obj284;
    lv_obj_t *obj285;
    lv_obj_t *obj286;
    lv_obj_t *obj287;
    lv_obj_t *obj288;
    lv_obj_t *obj289;
    lv_obj_t *obj290;
    lv_obj_t *obj291;
    lv_obj_t *obj292;
    lv_obj_t *obj293;
    lv_obj_t *obj294;
    lv_obj_t *obj295;
    lv_obj_t *obj296;
    lv_obj_t *obj297;
    lv_obj_t *obj298;
    lv_obj_t *obj299;
    lv_obj_t *obj300;
    lv_obj_t *obj301;
    lv_obj_t *obj302;
    lv_obj_t *obj303;
    lv_obj_t *obj304;
    lv_obj_t *obj305;
    lv_obj_t *obj306;
    lv_obj_t *obj307;
    lv_obj_t *obj308;
    lv_obj_t *obj309;
    lv_obj_t *obj310;
    lv_obj_t *obj311;
    lv_obj_t *obj312;
    lv_obj_t *obj313;
} objects_t;

extern objects_t objects;

typedef struct {
    lv_meter_scale_t *scale;
    lv_meter_scale_t *scale1;
    lv_meter_indicator_t *indicator;
    lv_meter_indicator_t *indicator1;
} screen_ex_tacho_state_t;

typedef struct {
    lv_meter_scale_t *scale;
    lv_meter_scale_t *scale1;
    lv_meter_indicator_t *indicator;
    lv_meter_indicator_t *indicator1;
} screen_ex_needle_state_t;

typedef struct {
    lv_meter_scale_t *scale;
    lv_meter_scale_t *scale1;
    lv_meter_scale_t *scale2;
    lv_meter_scale_t *scale3;
    lv_meter_scale_t *scale4;
    lv_meter_scale_t *scale5;
} screen_ex_gauges_state_t;

extern screen_ex_tacho_state_t screen_ex_tacho_state;
extern screen_ex_needle_state_t screen_ex_needle_state;
extern screen_ex_gauges_state_t screen_ex_gauges_state;

void create_screen_dash();
void tick_screen_dash();

void create_screen_race();
void tick_screen_race();

void create_screen_engine();
void tick_screen_engine();

void create_screen_ex_tacho();
void tick_screen_ex_tacho();

void create_screen_ex_needle();
void tick_screen_ex_needle();

void create_screen_ex_bars();
void tick_screen_ex_bars();

void create_screen_ex_gauges();
void tick_screen_ex_gauges();

void create_screen_ex_strip();
void tick_screen_ex_strip();

void create_screen_settings();
void tick_screen_settings();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/