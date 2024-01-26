//foto rez adc -- to test
//led status bicolor -- to test
//buton reset fabrica -- to test
// break after case
//anim12 done
//anim13 done



///////////hardware/////////
////EN - C:10uF - GND
/// pin 18 leds
///+3.3 - fotorez - pin34 - rez 20k -gnd

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

//#include <WiFi.h>////////////esp32
//#include <ESP8266WiFi.h>/////////esp8266
#include <WebSocketsServer.h>
#include <WebServer.h>////////////esp32
///#include <ESP8266WebServer.h> //////////esp8266
#include <ArduinoJson.h>
#include <EEPROM.h>

#define DEBUG_PRINTLN(...) { Serial.print(#__VA_ARGS__);Serial.print(": ");Serial.println(__VA_ARGS__); }
#define pir_down 19
#define pir_up 21
#define rst_pin 22
#define led1 14
#define led2 12//not good
#define adc_pin 34
#define max_leds 1550


WebServer server(80);////////////esp32
//ESP8266WebServer server(80);//////////esp8266
WebSocketsServer webSocket = WebSocketsServer(81);

int current;
long millis_held;
long secs_held;
long prev_secs_held;
byte previous = HIGH;
unsigned long firstTime;

unsigned long previousMillis_adc = 0;

byte ledState = LOW;
unsigned long previousMillis = 0;
int interval = 1000;

boolean f_send = false;
String c_anim = "0";
boolean zig;
boolean directlogic;
byte foto_rez, foto_hys = 3;
String foto_rez_s = String(foto_rez);
float adc;
int8_t adc_p;

byte anim_mode ;

String net_name, pass;
char net_name_s[20];
char pass_s[20];
byte ip1, ip2, ip3, ip4, msk1, msk2, msk3, msk4;
String ip1_s, ip2_s, ip3_s, ip4_s, msk1_s, msk2_s, msk3_s, msk4_s;
byte has_data;
IPAddress local_ip;
IPAddress gateway_ip;
IPAddress subnet_ip;
IPAddress apIP(10, 10, 10, 1);
String s;
String IPaddress;

uint8_t LineCount = 15;///////18;
uint16_t pixelPerLine = 60;///////////64;
String LineCount_s = String(LineCount);
String pixelPerLine_s = String(pixelPerLine);

uint16_t PixelCount = LineCount * pixelPerLine;
const uint8_t PixelPin = 18;

char SerialChar;

boolean timeToChangeAnimation;
boolean upa, downa, canGo;
uint32_t cda = 15000, cdb = 5000, EEPROM_CD = 500;
String cdb_s = String(cdb / 1000);

HsbColor R = HsbColor(0.0f, 1.0f, 0.5f);
HsbColor Y = HsbColor(0.0611f, 1.0f, 0.5f); //HsbColor Y = HsbColor(0.166f, 1.0f, 0.5f);
HsbColor B = HsbColor(0.66f, 1.0f, 0.5f);

// Proportii steag 5:3 ?
int steagW = 15, steagW_1 = 5; //int steagW = 15, steagW_1 = 5; // 5|5|5  R|Y|B
int steagH = 3;//int steagH = 9;

// anim1
float anim1_H, anim1_S, anim1_V, anim1_deltaH;
uint16_t anim1_time;
float anim1_H_d = 0.0f, anim1_S_d = 1.0f, anim1_V_d = 0.07f, anim1_deltaH_d = 0.01f;
uint16_t anim1_time_d = 500;
char * anim1_p[5] = {"Hue", "Saturation", "Intensity", "ΔHue", "Time [ms]"};

// anim2
uint16_t anim2_time_changeH = 20000, anim2_time_dimS = 300, anim2_time_moveH = 2000;
float anim2_V = 0.08f, anim2_fadeBy = 0.75f;
uint16_t anim2_time_changeH_d = 20000, anim2_time_dimS_d = 300, anim2_time_moveH_d = 2000;
float anim2_V_d = 0.08f, anim2_fadeBy_d = 0.75f;
char * anim2_p[5] = {"Time Hue [ms]", "Time Dim [ms]", "Time Move Hue [ms]", "Intensity", "Fade By"};

// anim3
uint8_t anim3_darkenBy = 10;
uint16_t anim3_time_scroll = 1000, anim3_time_fade = 40;
float anim3_V = 0.15f, anim3_deltaH = 0.008;
uint8_t anim3_darkenBy_d = 10;
uint16_t anim3_time_scroll_d = 1000, anim3_time_fade_d = 40;
float anim3_V_d = 0.15f, anim3_deltaH_d = 0.008;
char * anim3_p[5] = {"Darken By", "Time Scroll [ms]", "Time Fade [ms]", "Intensity", "ΔHue"};

// anim4
float anim4_H = 0.666f, anim4_S = 1.0f, anim4_V = 0.07f;
float anim4_H_d = 0.666f, anim4_S_d = 1.0f, anim4_V_d = 0.07f;
char* anim4_p[3] = {"Hue", "Saturation", "Intensity"};

// anim5
uint16_t anim5_t1 = 1100, anim5_t2 = 1300;
float anim5_maxV = 0.1f;
uint16_t anim5_t1_d = 1100, anim5_t2_d = 1300;
float anim5_maxV_d = 0.1f;
char* anim5_p[3] = {"Lower Time [ms]", "Upper Time [ms]", "Intensity"};

// anim6
uint16_t anim6_time_fade = 100, anim6_fadeBy = 10, anim6_t1 = 2000, anim6_t2 = 3000, anim6_P = 95;
float anim6_V = 0.07f;
uint16_t anim6_time_fade_d = 100, anim6_fadeBy_d = 10, anim6_t1_d = 2000, anim6_t2_d = 3000, anim6_P_d = 95;
float anim6_V_d = 0.07f;
char* anim6_p[6] = {"Time Fade [ms]", "Fade By", "Lower Time [ms]", "Upper Time[ms]", "P", "Intensity"};

// anim7
uint16_t anim7_time = 4000;
float anim7_V = 0.07f, anim7_deltaH = 0.05f;
uint16_t anim7_time_d = 4000;
float anim7_V_d = 0.07f, anim7_deltaH_d = 0.05f;
char* anim7_p[3] = {"Time [ms]", "Intensity", "ΔHue"};

// anim8
uint16_t anim8_t1 = 800, anim8_t2 = 1100;
float anim8_V = 0.05f;
uint16_t anim8_t1_d = 800, anim8_t2_d = 1100;
float anim8_V_d = 0.05f;
char* anim8_p[3] = {"Lower Time [ms]", "Upper Time [ms]", "Intensity"};

// anim9
uint16_t anim9_time = 5000;
float anim9_H = 0.4f, anim9_S = 1.0f, anim9_V = 0.12f;
uint16_t anim9_time_d = 5000;
float anim9_H_d = 0.3f, anim9_S_d = 0.1f, anim9_V_d = 0.1f;
char* anim9_p[4] = {"Hue", "Saturation", "Intensity" ,"Time [ms]"};

// anim10
uint16_t anim10_time = 3000;
float anim10_V = 0.1f, anim10_deltaT_p = 1.0f / float(LineCount), anim10_tau_p = anim10_deltaT_p * 0.5f, anim10_H = 0.0f, anim10_S = 1.0f;
uint16_t anim10_time_d = 3000;
float anim10_V_d = 0.1f, anim10_H_d = 0.0f, anim10_S_d = 1.0f;
char* anim10_p[4] = {"Hue", "Saturation", "Intensity", "Time [ms]"};

// anim11
uint16_t anim11_time = 5000;
float anim11_H = 0.0f, anim11_S = 1.0f, anim11_V = 0.07f;
uint16_t anim11_time_d = 5000;
float anim11_H_d = 0.0f, anim11_S_d = 1.0f, anim11_V_d = 0.07f;
char* anim11_p[4] = {"Hue", "Saturation", "Intensity", "Time [ms]"};

// anim12
uint16_t anim12_time = 4000, anim12_time_Up = 40;
float anim12_H = 0.0f, anim12_S  = 1.0f, anim12_V = 0.07f, anim12_upBy = 1.5f;
uint16_t anim12_time_d = 4000, anim12_time_Up_d = 40;
float anim12_H_d = 0.0f, anim12_S_d = 1.0f, anim12_V_d = 0.07f,anim12_upBy_d = 1.5f;// anim12_upBy_d = 1.5f;
char*anim12_p[6] = {"Hue", "Saturation", "Intensity", "Time [ms]", "UpBy", "Time UpBy [ms]"};

//anim13
uint16_t anim13_move_time = 300;
float anim13_V = 0.07f;
uint16_t anim13_move_time_d = 300;
float anim13_V_d = 0.07f;
char*anim13_p[2] = {"Intensity", "Move time[ms]"};

//anim14 
uint16_t anim14_move_time = 2000;
float anim14_V = 0.05f;
uint16_t anim14_move_time_d = 2000;
float anim14_V_d = 0.05f;
char*anim14_p[2] = {"Intensity", "Move time[ms]"};

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(max_leds, PixelPin);
NeoPixelAnimator animations(max_leds);

const char webpage_1[] PROGMEM = R"=====(
 <!DOCTYPE html>
 <html>
   <head>
     <meta http-equiv='content-type' content='text/html; charset=UTF-8'>
     <title>index</title>
     <script type='text/javascript'>
   document.addEventListener('keypress', function(event) {
         if (event.keyCode == 13) {
           event.preventDefault();
        }
    });

       function isNumber(evt) {
         evt = (evt) ? evt : window.event;
         var charCode = (evt.which) ? evt.which : evt.keyCode;
         if (charCode > 31 && (charCode < 48 || charCode > 57)) {
            return false;
            }
         return true;
         }
      function max_nr(val,name,max) {
         if(parseInt(val,10) > parseInt(max,10)){document.getElementsByName(name)[0].value=max     }
       }
      window.onbeforeunload = function (e) {
         
        }
      function start() {
        console.log("page nr: "+location.pathname.substring(location.pathname.lastIndexOf("/") + 1));
        websock = new WebSocket('ws://' + window.location.hostname + ':81/');
        //websock.binaryType = 'arraybuffer';
        websock.onopen = function(evt) {
          console.log('websock open');
          websock.send("start");
          }
        websock.onclose = function(evt) { 
          start();      
          console.log('websock close');
          };
        websock.onerror = function(evt) { console.log(evt); };
        websock.onmessage = function(event)
        {
          console.log(event);
          data = JSON.parse(event.data);
          if(data.ilum!= null){
            ilum=document.getElementById("ilum");
            ilum.textContent = data.ilum+"%";
          }
           if(data.rssi!= null){
            ilum=document.getElementById("rssi");
            ilum.textContent = data.rssi+"%";
          }
        }
       }
)=====";
const char webpage_2[] PROGMEM = R"=====( 
 </script> <style type='text/css'>
.right {
    float: right;
    clear: both;
    padding-right: 22ex;
    margin-left: -13ex;
}
 </style></head>
   <body onload="javascript:start();" style='       font-size:large;background-color: #ddeaff'> <button style='overflow: visible !important; height: 0 !important;   width: 0 !important; margin: 0 !important; border: 0
     !important; padding: 0 !important; display: block !important;' type='submit'
       value='save' formmethod='post' form='form1'> </button>
       <script>
    //if ( window.history.replaceState ) {
    //    window.history.replaceState( null, null, window.location.href );
    //}
    </script>
     <table ;='' style='width: 731px;' border='0' align='center'>
       <tbody>
     <form action="/settings.html" enctype="multipart/form-data" method='POST' id='form1'>
         <tr align='center'>
           <td style='width: 731px; height: 64.4px;background-color: #fff1d3'>
             <table style='width: 390px; height: 38px; text-align: left; margin-left: auto; margin-right: auto;'
               border='0'>
               <tbody>
                 <tr>
                   <td style='text-align: center; background-color: #1442e5; font-size: x-large;'><span
                       style='color: #0d0e0d;'><span style='color: white;'>Network
                         Settings</span><br>
                     </span> </td>
                 </tr>
               </tbody>
             </table>
             <table style='width: 390px; height: 56px;' border='0'>
               <tbody id='a'>
                 <tr>
                   <td style='text-align: left; width: 329.617px; height: 23px;'><span style='color: #0d0e0d;'>Network name:</span></td>
                   <td style='width: 131px;'><span style='color: #0d0e0d;'><input name='net_name' value='
)=====";
const char webpage_3[] PROGMEM = R"=====(' style='height: 16.8px;' type='text'><br>
                     </span> </td>
                 </tr>
                 <tr>
                   <td style='text-align: left;'><span style='color: #0d0e0d;'>Network
                       Password:</span></td>
                   <td><span style='color: #0d0e0d;'><input name='net_pass' type='password'><br>
                     </span> </td>
                 </tr>
               </tbody>
             </table>
           </td>
         </tr>
         <tr>
           <td style='width: 0px; background-color: white; text-align: center; height: 70.4px;'>
             <table style='width: 390px; text-align: left; margin-left: auto; margin-right: auto; height: 92px;'

               border='0'>
               <tbody>
                 <tr>
                   <td rowspan='1' colspan='2' id='tm' style='font-size: x-large; text-align: center; height: 28.8px; background-color: #1e971e;'><span

                       style='color: white;'>Device settings</span></td>
                 </tr>
                 <tr>
                   <td id='c' style='height: 14.2px; width: 91.75px;'>IP address:</td>
                   <td style='text-align: center; width: 255.25px;'> <input name='ip_val4' value='
)=====";                   
const char webpage_4[] PROGMEM = R"=====('size='2' maxlength='3' onkeypress='return isNumber(event)'
                       onkeyup="max_nr(this.value,name,'255')" type='text'>.
                     <input name='ip_val3' value=')=====";
const char webpage_5[] PROGMEM = R"=====(' size='2' maxlength='3' onkeypress='return isNumber(event)'

                       onkeyup="max_nr(this.value,name,'255')" style='height: 16.8px;'

                       type='text'>. <input name='ip_val2' value=')=====";
const char webpage_6[] PROGMEM = R"=====(' size='2' maxlength='3'

                       onkeypress='return isNumber(event)' onkeyup="max_nr(this.value,name,'255')" type='text'>. <input name='ip_val1' value=')=====";
const char webpage_7[] PROGMEM = R"=====(' size='2' maxlength='3'

                       onkeypress='return isNumber(event)' onkeyup="max_nr(this.value,name,'255')" type='text'><br>
                   </td>
                 </tr>
                 <tr>
                   <td>Subnet mask:</td>
                   <td style='text-align: center;'><input name='msk_val4' value=')=====";
const char webpage_8[] PROGMEM = R"=====(' size='2'
                       maxlength='3' onkeypress='return isNumber(event)' onkeyup="max_nr(this.value,name,'255')" type='text'>. <input name='msk_val3' value=')=====";
const char webpage_9[] PROGMEM = R"=====(' size='2' maxlength='3'

                       onkeypress='return isNumber(event)' onkeyup="max_nr(this.value,name,'255')" style='height: 16.8px;' type='text'>. <input name='msk_val2' value=')=====";
const char webpage_10[] PROGMEM = R"=====('size='2' maxlength='3' onkeypress='return isNumber(event)'
                       onkeyup="max_nr(this.value,name,'255')" type='text'>.
                     <input name='msk_val1' value=')=====";
const char webpage_11[] PROGMEM = R"=====(' size='2' maxlength='3' onkeypress='return isNumber(event)'

                       onkeyup="max_nr(this.value,name,'255')" type='text'></td>
                 </tr>
               </tbody>
               <tbody>
                 <tr>
                 </tr>
               </tbody>
             </table>
           </td>
         </tr>
         <tr>
             <table style='width: 250px; height: 66px;' border='0' align='center'>
               <tbody>
                 <tr>
                   <td rowspan='1' colspan='4' style='padding-left: 200px;font-size: x-large; background-color:white; height: 13px;'><span>
                        <span>Signal power:    </span><span id="rssi" style="color:#FF8A65;position:absolute;width:4rem;text-align:right;">
                       </span>
                       </td>
                 </tr>
                 
                 </tr>
                   <tr>
                   <td rowspan='1' colspan='4' style='padding-left: 200px;font-size: x-large; background-color:white; height: 13px;'><span>
                       <span>Number of steps:</span>
                       <span class="right">
                       <input name='steps' value=')=====";
const char webpage_12[] PROGMEM = R"=====(' size='2' onkeypress='return isNumber(event)'
                       onkeyup="max_nr(this.value,name,'25')" type='text'>
                       </span>
                       <br>
                       <span>Number of leds per steps:</span>
                       <span class="right">
                       <input name='nr_leds' value=')=====";
const char webpage_13[] PROGMEM = R"=====(' size='2' onkeypress='return isNumber(event)'
                       onkeyup="max_nr(this.value,name,'200')" type='text'>
                       </span>
                       <br>
                       <div>Maximum nr. of leds is 2000.</div>
                       </td>
                 </tr>
 </tr>
                 <tr>
                   <td rowspan='1' colspan='4' style='padding-left: 200px;font-size: x-large; background-color:white; height: 13px;'><span>
                       <input type="radio" id="zig" formmethod='post' form='form1' name="zig" value="zig">    Zig-zag
                       <input type="radio" id="no_zig" formmethod='post' form='form1' name="zig" value="no_zig"> No zig-zag<br>
                       </span>
                       </td>
                 </tr>
)=====";
const char webpage_14[] PROGMEM = R"=====(                 
        
                   <tr>
                   <td rowspan='1' colspan='4' style='padding-left: 200px;font-size: x-large; background-color:white; height: 13px;'><span>
                       <input type="radio" id="pir" formmethod='post' form='form1' name="pir" value="pir">Using sensors<br>
                       <input type="radio" id="no_pir" formmethod='post' form='form1' name="pir" value="no_pir">Not using sensors<br>
                       </span>
                       </td>
                 </tr>
                    </tr>
                   <tr>
                   <td rowspan='1' colspan='4' style='padding-left: 200px;font-size: x-large; background-color:white; height: 13px;'><span>
                       <input type="radio" id="directlogic" formmethod='post' form='form1' name="directlogic" value="directlogic">Direct logic sensors<br>
                       <input type="radio" id="no_directlogic" formmethod='post' form='form1' name="directlogic" value="no_directlogic">Inverse logic sensors<br>
                       </span>
                       </td>
                 </tr>
                   <tr>
                   <td rowspan='1' colspan='4' style='padding-left: 200px;font-size: x-large; background-color:white; height: 13px;'><span>
                       <span>Animation Duration: [s]</span>
                       <span class="right">
                       <input name='cdb' value=')=====";
const char webpage_15[] PROGMEM = R"=====(' size='2' onkeypress='return isNumber(event)'
                       onkeyup="max_nr(this.value,name,'1000')" type='text'>
                       </span>
                       </td>
                 </tr>
 </tr>
                   <tr>
                   <td rowspan='1' colspan='4' style="padding-left: 200px;font-size: x-large; background-color:white; height: 13px;"><span>
                       <span>Light sensor trigger:[%]</span>
                       <span class="right">
                       <input name='foto_rez' value=')=====";
const char webpage_16[] PROGMEM = R"=====(' size='2' onkeypress='return isNumber(event)'
                       onkeyup="max_nr(this.value,name,'100')" type='text'>
                       </span>
                       <br>
                       <span>Light sensor value:    </span><span id="ilum" style="color:#29B6F6;position:absolute;width:4rem;text-align:right;"></span>
                       </td>
                 </tr>                 
          <tr>
           <td style='background-color:#f7dbc0'>
            <table style='width: 738px; height: 93px;' ;background-color:white=''
              border='0'>
              <tbody>
                <tr>
                  <td style='text-align: center; width: 307px;'><button style='height: 70px; width: 85px; font-size: larger;'
                      name='save' type='submit' formmethod='post' form='form1' value='save'>Save</button></td>
                  <td style='text-align: center; width: 307px;'><button style='height: 70px; width: 154px; font-size: larger;'
                      name='reset' type='submit' formmethod='post' form='form1'
                      value='reset'>Reset and connect</button></td>
                  <td style='text-align: center; width: 307px;'>
                  <button id="backbutton" type="button" onclick='location.href="/"' style='height: 70px; width: 85px; font-size: larger;' >Back</button><br>
                  </td>
                 </tr>
               </tbody>
             </table>
           </td>
         </tr>
       </tbody>
     </table>
     <br>
     <br>
     <div style='left: 868px; top: 8px; height: 476.683px;'></div>
    <script>
    var zig =")=====";
const char webpage_17[] PROGMEM = R"=====("
var pir =")=====";
const char webpage_18[] PROGMEM = R"=====("
var directlogic =")=====";
const char webpage_19[] PROGMEM = R"=====("
 //if(zig=="zig") document.getElementById("zig").checked="checked";
   // if(zig=="no_zig") document.getElementById("no_zig").checked="checked"; 
    document.getElementById(zig).checked="checked";
    document.getElementById(pir).checked="checked";   
    document.getElementById(directlogic).checked="checked";      
     </script>
   </body>
 </html>
)=====";

static char PROGMEM anim_page_1[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>

<style>
.button {
  background-color: #4CAF50;
  border: none;
  color: white;
  padding: 0.5% 2%;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 2em;
  //font-size: 1.7vi;
  margin: 10px 2px;
  -webkit-transition-duration: 0.4s; 
  transition-duration: 0.4s;
  cursor: pointer;
  
}
.button1 {
  background-color: white; 
  color: black; 
  border: 2px solid #4CAF50;
  width: 15%;
  height: 30px;
}
.button1:hover {
  background-color: #A5D6A7;
  color: white;
}
.button1:active {
  background-color: #43A047;
  color: white;
  wheight: 16%;
}

.slidecontainer {
  width: 100%;
}
.slider {
  -webkit-appearance: none;
  width: 40%;
  height: 25px;
  background: #d3d3d3;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}

.slider:hover {
  opacity: 1;
}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 25px;
  height: 25px;
  background: #4CAF50;
  cursor: pointer;
}

.slider::-moz-range-thumb {
  width: 25px;
  height: 25px;
  background: #4CAF50;
  cursor: pointer;
}
</style>
<script>

//var data;
    function hue(h,s) {
      var b = document.body;
      var c = document.getElementById("cvs");
      var a = c.getContext('2d');
      document.body.clientWidth; 
    var doc = document;
    doc.c = doc.createElement;
    b.a = b.appendChild;    
    var width = c.width = c.height = 200,
        hue = b.a(doc.c("div")),
        sat = b.a(doc.c("div"));
        imageData = a.createImageData(width, width),
        pixels = imageData.data,
        //oneHundred = input.value = input.max = 100,
        circleOffset = 10,
        diameter = width-circleOffset*2,                  
        radius = diameter / 2,                    
        radiusPlusOffset = radius + circleOffset,          
        radiusSquared = radius * radius,
        currentY = 100,
        currentX = -currentY,
        wheelPixel = circleOffset*4*width+circleOffset*4; 
    var math = Math,
        PI = math.PI,
        PI2 = PI * 2,
        sqrt = math.sqrt,
        atan2 = math.atan2;
        //h=   (((theta + PI) / PI2)*360).toFixed(0);       // Current hue (how many degrees along the circle)
      // sat.textContent ="Saturation: " + ((sqrt(d) / radius)*100).toFixed(0)
        var theta = h*PI2-PI;
        var d = Math.pow(s*radius,2);
        console.log("h= "+h);
         console.log("s= "+s);
         console.log("theta= "+theta);
         console.log("d= "+d);
       //   h =(theta + PI) / PI2,        
       //   s = sqrt(d) / radius,
 )rawliteral";
static char PROGMEM anim_page_2[] = R"rawliteral(
    b.style.textAlign="left";
    //hue.style.font = "2em courier";
    //input.type = "range";
    for (y  = 0; y < width; y++) {
        for (x = 0; x < width; x++) {
            var rx = x - radius,
                ry = y - radius,
                d = rx * rx + ry * ry,
                rgb = hsvToRgb(
                    (atan2(ry, rx) + PI) / PI2,     // Hue
                    sqrt(d) / radius,               // Saturation
                    1                               // Value
                );
            pixels[wheelPixel++] = rgb[0];
            pixels[wheelPixel++] = rgb[1];
            pixels[wheelPixel++] = rgb[2];
            pixels[wheelPixel++] = d > radiusSquared ? 0 : 255;
        }
    }
    //input.onchange = redraw;
    c.onmousedown = doc.onmouseup = function(e) {
        
        //doc.onmousemove = /p/.test(e.type) ? 0 : (redraw(e), redraw);
        doc.onmouseclick = /p/.test(e.type) ? 0 : (redraw(e), redraw);
    }

    function redraw(e) {
        currentX = e.pageX - c.offsetLeft - radiusPlusOffset || currentX;
        currentY = e.pageY - c.offsetTop - radiusPlusOffset  || currentY;
        //var theta = atan2(currentY, currentX),
            theta = atan2(currentY, currentX);
            d = currentX * currentX + currentY * currentY;
        if (d > radiusSquared) {
            currentX = radius * math.cos(theta);
            currentY = radius * math.sin(theta);
            theta = atan2(currentY, currentX);
            d = currentX * currentX + currentY * currentY;
        }
        
        //label.textContent = b.style.background = hsvToRgb(
      //sat.style.background = val.style.background = hue.style.background = hsvToRgb(
      document.getElementById("col").style.background  = hsvToRgb(
         (theta + PI) / PI2,         // Current hue (how many degrees along the circle)
          sqrt(d) / radius,           // Current saturation (how close to the middle)
          1//input.value / oneHundred    // Current value (input type="range" slider value)
        )[3];
)rawliteral";
static char PROGMEM anim_page_3[] = R"rawliteral(
       hue.textContent ="Hue: " + (((theta + PI) / PI2)*360).toFixed(0);       // Current hue (how many degrees along the circle)
       sat.textContent ="Saturation: " + ((sqrt(d) / radius)*100).toFixed(0) + "%";        // Current saturation (how close to the middle)
       data["Hue"] =  (((theta + PI) / PI2)*360).toFixed(0);
       data["Saturation"] = (sqrt(d) / radius).toFixed(2);
       websock.send(JSON.stringify(data));
       //val.textContent ="Value: "+((input.value / oneHundred)*100).toFixed(0)+"%";    // Current value (input type="range" slider value)
        // Reset to color wheel and draw a spot on the current location. 
        a.putImageData(imageData, 0, 0);
        // Circle:
        a.beginPath();  
        a.strokeStyle = '#000';
        a.arc(~~currentX+radiusPlusOffset,~~currentY+radiusPlusOffset, 4, 0, PI2);
        a.stroke();     
    }
     function hsvToRgb(h1, s1, v1) {
        h1*=6;
        var i = ~~h1,
            f = h1 - i,
            p = v1 * (1 - s1),
            q = v1 * (1 - f * s1),
            t = v1 * (1 - (1 - f) * s1),
            mod = i % 6,
            r = [v1, q, p, p, t, v1][mod] * 255,
            g = [t, v1, v1, q, p, p][mod] * 255,
            b = [p, p, t, v1, v1, q][mod] * 255;
        return [r, g, b, "rgb("+ ~~r + "," + ~~g + "," + ~~b + ")"];
    }
    //redraw(0);
      document.getElementById("col").style.background  = hsvToRgb(h/360,s,1)[3];
       hue.textContent ="Hue: " + h;       // Current hue (how many degrees along the circle)
       sat.textContent ="Saturation: " + (s*100).toFixed(0) + "%";        // Current saturation (how close to the middle)
       //data["Hue"] =  (((theta + PI) / PI2)*360).toFixed(0);
      // data["Saturation"] = (sqrt(d) / radius).toFixed(2);
      // websock.send(JSON.stringify(data));
       //val.textContent ="Value: "+((input.value / oneHundred)*100).toFixed(0)+"%";    // Current value (input type="range" slider value)
        a.putImageData(imageData, 0, 0);
 )rawliteral";
static char PROGMEM anim_page_4[] = R"rawliteral(
}
 function change(id, value) {
    console.log("change:.........");
    var val = document.getElementById("val"+id);
    val.innerText = id + ": " + value;
    if(id == "Intensity") val.innerText += "%";
    //val.innerHtml = id + ": " + value;
    //console.log(id+": "+value);
    //data[id] = value;
    //websock.send(JSON.stringify(data));
   }
 function send(id, value) {
    console.log(id+": "+value);
    data[id] = value;
    websock.send(JSON.stringify(data));
   }
 function start() {
  console.log("page nr: "+location.pathname.substring(location.pathname.lastIndexOf("/") + 1));
  websock = new WebSocket('ws://' + window.location.hostname + ':81/');
  websock.binaryType = 'arraybuffer';
  websock.onopen = function(evt) {
    websock.send(location.pathname.substring(location.pathname.lastIndexOf("/") + 1));
    console.log('websock open');
    }
  websock.onclose = function(evt) { 
        start();
        console.log('websock close');};
  websock.onerror = function(evt) { console.log(evt); };
  websock.onmessage = function(event)
  {
    console.log(event);
    data = JSON.parse(event.data);
   var c=0;
   for(var i in data){ 
    if(c===0) {
      document.getElementById("name").innerText="Animation "+data.anim;
    } else
    if (c==1 && data.color == "color"){
      console.log("color");
      hue(data["Hue"],data["Saturation"]);
    } else
    
    if (i!="Hue" && i!="Saturation")
    {
      var span = document.createElement('p');
      span.setAttribute("id", "val"+i);
      span.innerText  = i+": "+data[i];
      if(i=="Intensity")span.innerText += "%";
      document.body.appendChild(span);
      var range = document.createElement('input');
      range.setAttribute("type", "range");
      range.setAttribute("class", "slider");
      range.setAttribute("id", i);
      range.setAttribute("oninput", "change(this.id, this.value)");
      range.setAttribute("onchange", "send(this.id, this.value)");
      // range.oninput = function(){change(this.id, this.value)};
      var step;
      var max;
 )rawliteral";
static char PROGMEM anim_page_5[] = R"rawliteral(
     if(data[i]<1){
      step=0.01;
      max=1;
        } else
     if(data[i]>=1 && data[i]<100){
         step=1;
         max=100;
        } else
     if(data[i]>=100 && data[i]<1000){
        step=1;
        max=1000;
        } else
     if(data[i]>=1000 && data[i]<10000){
      step=1;
      max=10000;
     }
     if(data[i]>=10000){
        step=10;
        max=32000;
        }
      range.setAttribute("max", max);
      range.setAttribute("step", step);
      console.log("step= " + step);   
      range.setAttribute("min", step);
        if(i=="Intensity"){
       range.setAttribute("max", 100);
       range.setAttribute("step", 1);
       range.setAttribute("min", 0);
        } 
      range.setAttribute("value", data[i]);
      console.log("data[i]+: "+i+"= "+ data[i]);   
      document.body.appendChild(range);
      //document.getElementById(i).addEventListener("oninput",change(this.id, this.value))
      console.log(document.getElementById(i).value);
    }
    c=c+1;
   }
  }
 }
</script>
</head>
<body onload="javascript:start();">
<form action="" method='POST' id='form1'></form>
<script>
var data;
</script>
<h1 id = "name">Animation</h1>
<!--//<button id= "default" class="button button1"  name='default' type='submit' formmethod='post' form='form1' value='default'>Default</button>
-->
<button id= "default" class="button button1"  type='submit' formmethod='post' form='form1' value='default'">Default</button>
<script>
document.getElementById("default").name=location.pathname.substring(location.pathname.lastIndexOf("/") + 1);
</script>
<button id= "backbutton" class="button button1" onclick="location.href='/'">Back</button><br>

<p id = "col" style= "margin:1%; padding:0;"><br></p>

<canvas id="cvs" width="0" height="0"> </canvas>
</body>
</html>
)rawliteral";
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const char PROGMEM INDEX_HTML_1[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-16">
    
    <title>eps32anim</title>
    <style>
.button {
  background-color: #4CAF50; /* Green */
  border: none;
  color: white;
  padding: 0.5% 2%;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 2.8vw;
  //font-size: 1.7vi;
  margin: 4px 2px;
  -webkit-transition-duration: 0.4s; /* Safari */
  transition-duration: 0.4s;
  cursor: pointer;
  height: 12%;
  width: 25%;
}

.button1 {
  background-color: white; 
  color: black; 
  border: 2px solid #4CAF50;
}

.button1:hover {
  background-color: #4CAF50;
  color: white;
}

.button2 {
  background-color: white; 
  color: black; 
  border: 2px solid #008CBA;
}

.button2:hover {
  background-color: #008CBA;
  color: white;
}

</style> </head>
<body>
    <button id= "1" class="button button1" onclick="location.href='anim01.html'">Animation 1</button>
    <button id= "2" class="button button2" onclick="location.href='anim02.html'">Animation 2</button>
    <button id= "3" class="button button1" onclick="location.href='anim03.html'">Animation 3</button><br>
    <button id= "4" class="button button2" onclick="location.href='anim04.html'">Animation 4</button>
    <button id= "5" class="button button1" onclick="location.href='anim05.html'">Animation 5</button>
    <button id= "6" class="button button2" onclick="location.href='anim06.html'">Animation 6</button><br>
    <button id= "7" class="button button1" onclick="location.href='anim07.html'">Animation 7</button>
    <button id= "8" class="button button2" onclick="location.href='anim08.html'">Animation 8</button>
    <button id= "9" class="button button1" onclick="location.href='anim09.html'">Animation 9</button><br>
    <button id= "10" class="button button2" onclick="location.href='anim10.html'">Animation 10</button>
    <button id= "11" class="button button1" onclick="location.href='anim11.html'">Animation 11</button>
    <button id= "12" class="button button2" onclick="location.href='anim12.html'">Animation 12</button><br>
    <button id= "13" class="button button1" onclick="location.href='anim13.html'">Animation 13</button>
    <button id= "14" class="button button2" onclick="location.href='anim14.html'">Animation 14</button><br>
    <button id= "0" class="button button2" style="border-color:#FF3D00;" onclick="location.href='/?stop=stop'">Stop</button><br>
    <button id= "0" class="button button1" style="border-color:#651FFF;" onclick="location.href='/settings.html'">Settings</button><br>
    <!--
//    <form action="/" method="POST" id="form1">
//    <button id= "0" type="submit" class="button button2" name="stop" value="stop" formmethod='post' form=form1>Stop</button><br>
//    </form>
//    -->
  <script>
   if(performance.navigation.type == 2){
   location.href='/'
    }
   document.getElementById(")rawliteral";
static const char PROGMEM INDEX_HTML_2[] = R"rawliteral(").style.backgroundColor = "#FF1744";
  </script>
</body></html>
)rawliteral";

/*
void split_page()
{        char * wp=anim_page;
     //char buffer[5001] = {0};
    size_t len = strlen(wp); 
    Serial.print("len: ");Serial.println(len);
   
    size_t blen = sizeof(anim_split[0])-1;

    Serial.print("blen: ");Serial.println(blen);
    size_t i = 0;
    Serial.print("len/blen:  ");Serial.println(len/blen);
    
   for (i=0; i<len/blen; ++i){
        memcpy(anim_split[i], anim_page + (i*blen), blen);
    }
     Serial.print("i: ");Serial.println(i);
      
    if (len % blen){
      strcpy(anim_split[i], anim_page + (len - len % blen));
      i++;
    } 
 anim_size=i;

    Serial.print("anim array size: ");Serial.println("faf");
}
*/

void handleRoot()
{ 
  f_send = false;
  if (server.args() > 0 ) { // Arguments were received
  // Serial.println(server.argName(0));
    if (server.argName(0) == "stop"){
      c_anim="0";
      SerialChar = '0';
      timeToChangeAnimation=true;
    }
  }
//  Serial.print("c anim root: ");Serial.println(c_anim);
  //server.send_P(200, "text/html", INDEX_HTML);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendContent_P(INDEX_HTML_1);
  server.sendContent(c_anim);
  server.sendContent_P(INDEX_HTML_2);
  server.sendContent("");
  server.client().stop();
}

void save_eeprom_net(){
  Serial.println(F("Save to eeprom"));
  EEPROM.begin(256);
  byte x = 1;
  EEPROM.put(4,ip1);
  EEPROM.put(5,ip2);
  EEPROM.put(6,ip3);
  EEPROM.put(7,ip4);
  EEPROM.put(8,msk1);
  EEPROM.put(9,msk2);
  EEPROM.put(10,msk3);
  EEPROM.put(11,msk4);

  EEPROM.put(12,zig);
  EEPROM.put(13,anim_mode);
  EEPROM.put(14,directlogic);
  EEPROM.put(15,foto_rez);
  int addr = 16;
  EEPROM.put(addr,cdb);addr += sizeof(cdb);
  EEPROM.put(addr,LineCount);addr += sizeof(LineCount);
  EEPROM.put(addr,pixelPerLine);addr += sizeof(pixelPerLine);

  
  EEPROM.put(addr,net_name.length());addr += sizeof(net_name.length());
  EEPROM.put(addr,pass.length()); addr += sizeof(pass.length());
  for(byte i=0;i<net_name.length();i++)EEPROM.put(i+addr,net_name[i]);
  for(byte i=0;i<pass.length();i++)EEPROM.put(i+addr+net_name.length(),pass[i]);
  EEPROM.put(0,x);
  EEPROM.end();
//  Serial.println(String("TO EEPROM: ")+ ip1+" "+ip2+" "+ip3+" "+ip4+" "+msk1+" "+msk2+" "+msk3+" "+msk4+" "+net_name.length()+" "+net_name+" "+pass.length()+" "+pass);
}

void save_data_ee(){
  EEPROM.begin(1024);
  int addr=256;
  EEPROM.put(addr, SerialChar);addr += sizeof(SerialChar);
  EEPROM.put(addr, anim1_H);addr += sizeof(anim1_H);
  EEPROM.put(addr, anim1_S);addr += sizeof(anim1_S);
  EEPROM.put(addr, anim1_V);addr += sizeof(anim1_V);
  EEPROM.put(addr, anim1_deltaH);addr += sizeof(anim1_deltaH);
  EEPROM.put(addr,  anim1_time);addr += sizeof(anim1_time);

  EEPROM.put(addr,anim2_time_changeH);addr += sizeof(anim2_time_changeH);
  EEPROM.put(addr,anim2_time_dimS);addr += sizeof(anim2_time_dimS);
  EEPROM.put(addr,anim2_time_moveH);addr += sizeof(anim2_time_moveH);
  EEPROM.put(addr,anim2_V);addr += sizeof(anim2_V);
  EEPROM.put(addr,anim2_fadeBy);addr += sizeof(anim2_fadeBy);
  
  EEPROM.put(addr,anim3_darkenBy);addr += sizeof(anim3_darkenBy);
  EEPROM.put(addr,anim3_time_scroll);addr += sizeof(anim3_time_scroll);
  EEPROM.put(addr,anim3_time_fade);addr += sizeof(anim3_time_fade);
  EEPROM.put(addr,anim3_V);addr += sizeof(anim3_V);
  EEPROM.put(addr,anim3_deltaH);addr += sizeof(anim3_deltaH);

  EEPROM.put(addr,anim4_H);addr+=sizeof(anim4_H);
  EEPROM.put(addr,anim4_S);addr+=sizeof(anim4_S);
  EEPROM.put(addr,anim4_V);addr+=sizeof(anim4_V);

  EEPROM.put(addr,anim5_t1);addr+=sizeof(anim5_t1);
  EEPROM.put(addr,anim5_t2);addr+=sizeof(anim5_t2);
  EEPROM.put(addr,anim5_maxV);addr+=sizeof(anim5_maxV);

  EEPROM.put(addr,anim6_time_fade);addr+=sizeof(anim6_time_fade);
  EEPROM.put(addr,anim6_fadeBy);addr+=sizeof(anim6_fadeBy);
  EEPROM.put(addr,anim6_t1);addr+=sizeof(anim6_t1);
  EEPROM.put(addr,anim6_t2);addr+=sizeof(anim6_t2);
  EEPROM.put(addr,anim6_P);addr+=sizeof(anim6_P);
  EEPROM.put(addr,anim6_V);addr+=sizeof(anim6_V);

  EEPROM.put(addr,anim7_time);addr+=sizeof(anim7_time);
  EEPROM.put(addr,anim7_V);addr+=sizeof(anim7_V);
  EEPROM.put(addr,anim7_deltaH);addr+=sizeof(anim7_deltaH);

  EEPROM.put(addr,anim8_t1);addr+=sizeof(anim8_t1);
  EEPROM.put(addr,anim8_t2);addr+=sizeof(anim8_t2);
  EEPROM.put(addr,anim8_V);addr+=sizeof(anim8_V);

  EEPROM.put(addr,anim9_H);addr+=sizeof(anim9_H);
  EEPROM.put(addr,anim9_S);addr+=sizeof(anim9_S);
  EEPROM.put(addr,anim9_V);addr+=sizeof(anim9_V);
  EEPROM.put(addr,anim9_time);addr+=sizeof(anim9_time);

  EEPROM.put(addr,anim10_H);addr+=sizeof(anim10_H);
  EEPROM.put(addr,anim10_S);addr+=sizeof(anim10_S);
  EEPROM.put(addr,anim10_V);addr+=sizeof(anim10_V);
  EEPROM.put(addr,anim10_time);addr+=sizeof(anim10_time);

  EEPROM.put(addr,anim11_H);addr+=sizeof(anim11_H);
  EEPROM.put(addr,anim11_S);addr+=sizeof(anim11_S);
  EEPROM.put(addr,anim11_V);addr+=sizeof(anim11_V);
  EEPROM.put(addr,anim11_time);addr+=sizeof(anim11_time);

  EEPROM.put(addr,anim12_H);addr+=sizeof(anim12_H);
  EEPROM.put(addr,anim12_S);addr+=sizeof(anim12_S);
  EEPROM.put(addr,anim12_V);addr+=sizeof(anim12_V);
  EEPROM.put(addr,anim12_time);addr+=sizeof(anim12_time);
  EEPROM.put(addr,anim12_time_Up);addr+=sizeof(anim12_time_Up);
  EEPROM.put(addr,anim12_upBy);addr+=sizeof(anim12_upBy);  
  
  EEPROM.put(addr,anim13_move_time);addr+=sizeof(anim13_move_time);  
  EEPROM.put(addr,anim13_V);addr+=sizeof(anim13_V);  

  EEPROM.put(addr,anim14_move_time);addr+=sizeof(anim14_move_time);  
  EEPROM.put(addr,anim14_V);addr+=sizeof(anim14_V);  
  
  EEPROM.end();
  anim10_deltaT_p = 1.0f/float(LineCount);
  anim10_tau_p = anim10_deltaT_p*0.5f;
}
void load_eeprom_net(){
  unsigned long ct=millis();
  EEPROM.begin(256);
  byte b;
  EEPROM.get(0,b);
  if(b!=1){
    Serial.println(String("Expected 1, got: ")+b);
    Serial.println("Nothing to load");
    return;
  }
  Serial.println(F("Load from eeprom"));
  
  EEPROM.get(4,ip1);
  ip1_s=String(ip1);
  EEPROM.get(5,ip2);
  ip2_s=String(ip2);
  EEPROM.get(6,ip3);
  ip3_s=String(ip3);
  EEPROM.get(7,ip4);
  ip4_s=String(ip4);
  EEPROM.get(8,msk1);
  msk1_s=String(msk1);
  EEPROM.get(9,msk2);
  msk2_s=String(msk2);
  EEPROM.get(10,msk3);
  msk3_s=String(msk3);
  EEPROM.get(11,msk4);
  msk4_s=String(msk4);
  
  EEPROM.get(12,zig);
  EEPROM.get(13,anim_mode);
  EEPROM.get(14,directlogic);
  EEPROM.get(15,foto_rez);
  int addr = 16;
  EEPROM.get(addr,cdb);addr += sizeof(cdb);
  EEPROM.get(addr,LineCount);addr += sizeof(LineCount);
  EEPROM.get(addr,pixelPerLine);addr += sizeof(pixelPerLine);
  PixelCount = LineCount * pixelPerLine;
  //itoa(foto_rez,foto_rez_s,10);  
  foto_rez_s = String(foto_rez); 
  cdb_s = String(cdb/1000); 
  LineCount_s = String(LineCount);   
  pixelPerLine_s = String(pixelPerLine);                            
  uint32_t x1,x2;
  EEPROM.get(addr,x1); addr += sizeof(x1);
  EEPROM.get(addr,x2);  addr += sizeof(x2);
//  Serial.println("Loaded x1");
  for(uint32_t i=0;i<x1;i++){
    char c;
    EEPROM.get(i+addr,c);
    net_name+=c;
    if(millis()-ct>EEPROM_CD){
      EEPROM.put(0,0);
      return;
    }
  }
//  Serial.println("Loaded x2");
  for(uint32_t i=0;i<x2;i++){
    char c;
    EEPROM.get(i+addr+x1,c);
    pass+=c;
    if(millis()-ct>EEPROM_CD){
      EEPROM.put(0,0);
      return;
    }
  }
  EEPROM.end();
  net_name.toCharArray(net_name_s,20);
  pass.toCharArray(pass_s,20);
//  Serial.println("Loaded x3");
  Serial.println("From EEPROM:");
     Serial.println(String(ip4)+" "+ip3+" "+ip2+" "+ip1+" "+msk4+" "+msk3+" "+msk2+" "+msk1+" "+net_name+" "+pass);
}
void load_eeprom_data(){
  int addr=256;
  EEPROM.begin(1024);
  EEPROM.get(addr, SerialChar);addr += sizeof(SerialChar);
  c_anim = String(SerialChar);
  if(SerialChar=='a') c_anim="10";
  if(SerialChar=='c') c_anim="11";
  if(SerialChar=='g') c_anim="12";
   if(SerialChar=='j') c_anim="13";
  Serial.print("serial char: ");Serial.println(SerialChar);
  Serial.print("c_anim ");Serial.println(c_anim);
  EEPROM.get(addr, anim1_H);addr += sizeof(anim1_H);
  EEPROM.get(addr, anim1_S);addr += sizeof(anim1_S);
  EEPROM.get(addr, anim1_V);addr += sizeof(anim1_V);
  EEPROM.get(addr, anim1_deltaH);addr += sizeof(anim1_deltaH);
  EEPROM.get(addr, anim1_time);addr += sizeof(anim1_time);

  EEPROM.get(addr, anim2_time_changeH);addr += sizeof(anim2_time_changeH);
  EEPROM.get(addr, anim2_time_dimS);addr += sizeof(anim2_time_dimS);
  EEPROM.get(addr, anim2_time_moveH);addr += sizeof(anim2_time_moveH);
  EEPROM.get(addr, anim2_V);addr += sizeof(anim2_V);
  EEPROM.get(addr, anim2_fadeBy);addr += sizeof(anim2_fadeBy);
  
  EEPROM.get(addr, anim3_darkenBy);addr += sizeof(anim3_darkenBy);
  EEPROM.get(addr, anim3_time_scroll);addr += sizeof(anim3_time_scroll);
  EEPROM.get(addr, anim3_time_fade);addr += sizeof(anim3_time_fade);
  EEPROM.get(addr, anim3_V);addr += sizeof(anim3_V);
  EEPROM.get(addr, anim3_deltaH);addr += sizeof(anim3_deltaH);

  EEPROM.get(addr,anim4_H);addr+=sizeof(anim4_H);
  EEPROM.get(addr,anim4_S);addr+=sizeof(anim4_S);
  EEPROM.get(addr,anim4_V);addr+=sizeof(anim4_V);

  EEPROM.get(addr,anim5_t1);addr+=sizeof(anim5_t1);
  EEPROM.get(addr,anim5_t2);addr+=sizeof(anim5_t2);
  EEPROM.get(addr,anim5_maxV);addr+=sizeof(anim5_maxV);

  EEPROM.get(addr,anim6_time_fade);addr+=sizeof(anim6_time_fade);
  EEPROM.get(addr,anim6_fadeBy);addr+=sizeof(anim6_fadeBy);
  EEPROM.get(addr,anim6_t1);addr+=sizeof(anim6_t1);
  EEPROM.get(addr,anim6_t2);addr+=sizeof(anim6_t2);
  EEPROM.get(addr,anim6_P);addr+=sizeof(anim6_P);
  EEPROM.get(addr,anim6_V);addr+=sizeof(anim6_V);

  EEPROM.get(addr,anim7_time);addr+=sizeof(anim7_time);
  EEPROM.get(addr,anim7_V);addr+=sizeof(anim7_V);
  EEPROM.get(addr,anim7_deltaH);addr+=sizeof(anim7_deltaH);

  EEPROM.get(addr,anim8_t1);addr+=sizeof(anim8_t1);
  EEPROM.get(addr,anim8_t2);addr+=sizeof(anim8_t2);
  EEPROM.get(addr,anim8_V);addr+=sizeof(anim8_V);

  EEPROM.get(addr,anim9_H);addr+=sizeof(anim9_H);
  EEPROM.get(addr,anim9_S);addr+=sizeof(anim9_S);
  EEPROM.get(addr,anim9_V);addr+=sizeof(anim9_V);
  EEPROM.get(addr,anim9_time);addr+=sizeof(anim9_time);

  EEPROM.get(addr,anim10_H);addr+=sizeof(anim10_H);
  EEPROM.get(addr,anim10_S);addr+=sizeof(anim10_S);
  EEPROM.get(addr,anim10_V);addr+=sizeof(anim10_V);
  EEPROM.get(addr,anim10_time);addr+=sizeof(anim10_time);

  EEPROM.get(addr,anim11_H);addr+=sizeof(anim11_H);
  EEPROM.get(addr,anim11_S);addr+=sizeof(anim11_S);
  EEPROM.get(addr,anim11_V);addr+=sizeof(anim11_V);
  EEPROM.get(addr,anim11_time);addr+=sizeof(anim11_time);

  EEPROM.get(addr,anim12_H);addr+=sizeof(anim12_H);
  EEPROM.get(addr,anim12_S);addr+=sizeof(anim12_S);
  EEPROM.get(addr,anim12_V);addr+=sizeof(anim12_V);
  EEPROM.get(addr,anim12_time);addr+=sizeof(anim12_time);
  EEPROM.get(addr,anim12_time_Up);addr+=sizeof(anim12_time_Up);
  EEPROM.get(addr,anim12_upBy);addr+=sizeof(anim12_upBy);  

  EEPROM.get(addr,anim13_move_time);addr+=sizeof(anim13_move_time);  
  EEPROM.get(addr,anim13_V);addr+=sizeof(anim13_V);  

  EEPROM.get(addr,anim14_move_time);addr+=sizeof(anim14_move_time);  
  EEPROM.get(addr,anim14_V);addr+=sizeof(anim14_V);  
  
  EEPROM.end();
//  Serial.println("Loaded x4");

  anim10_deltaT_p = 1.0f/float(LineCount);
  anim10_tau_p = anim10_deltaT_p*0.5f;
}
void force_reset(){
  ESP.restart();
}

void handle_anim()
{
  f_send = false;
  // Serial.print("c anim anim: ");Serial.println(c_anim);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", anim_page_1);
  //server.sendContent(anim_page_1);
  server.sendContent(anim_page_2);
  server.sendContent(anim_page_3);
  server.sendContent(anim_page_4);
  server.sendContent(anim_page_5);
  server.sendContent("");
  server.client().stop();
  /*
  Serial.print("anim page 1: ");
  Serial.println(sizeof(anim_page_1));
  Serial.print("anim page 2: ");
  Serial.println(sizeof(anim_page_2));
  Serial.print("anim page 3: ");
  Serial.println(sizeof(anim_page_3));
  Serial.print("anim page 4: ");
  Serial.println(sizeof(anim_page_4));
  Serial.print("anim page 5: ");
  Serial.println(sizeof(anim_page_5));
  */
  if (server.args() > 0 ) {
    if(server.arg(0) == "default"){
      if(server.argName(0) == "anim01.html"){
        anim1_H=anim1_H_d;
        anim1_S=anim1_S_d;
        anim1_V=anim1_V_d;
        anim1_deltaH = anim1_deltaH_d;
        anim1_time = anim1_time_d;
      }
      if(server.argName(0) == "anim02.html"){
        anim2_time_changeH = anim2_time_changeH_d;
        anim2_time_dimS = anim2_time_dimS_d;
        anim2_time_moveH = anim2_time_moveH_d;
        anim2_V = anim2_V_d;
        anim2_fadeBy = anim2_fadeBy_d;
      }
      if(server.argName(0) == "anim03.html"){
        anim3_darkenBy = anim3_darkenBy_d;
        anim3_time_scroll = anim3_time_scroll_d;
        anim3_time_fade = anim3_time_fade_d;
        anim3_V = anim3_V_d;
        anim3_deltaH = anim3_deltaH_d;
      }
      if(server.argName(0) == "anim04.html"){
        anim4_H = anim4_H_d;
        anim4_S = anim4_S_d;
        anim4_V = anim4_V_d;
        
      }
      if(server.argName(0) == "anim05.html"){
        anim5_t1 = anim5_t1_d;
        anim5_t2 = anim5_t2_d;
        anim5_maxV = anim5_maxV_d;
      }
      if(server.argName(0) == "anim06.html"){
        anim6_time_fade = anim6_time_fade_d;
        anim6_fadeBy = anim6_fadeBy_d;
        anim6_t1 = anim6_t1_d; 
        anim6_t2 = anim6_t2_d;
        anim6_P = anim6_P_d;
        anim6_V = anim6_V_d;
      }
      if(server.argName(0) == "anim07.html"){
        anim7_time = anim7_time_d;
        anim7_V = anim7_V_d;
        anim7_deltaH = anim7_deltaH_d;
      }
      if(server.argName(0) == "anim08.html"){
        anim8_t1 = anim8_t1_d;
        anim8_t2 = anim8_t2_d;
        anim8_V = anim8_V_d;
      }
      if(server.argName(0) == "anim09.html"){
        anim9_H = anim9_H_d;
        anim9_S = anim9_S_d;
        anim9_V = anim9_V_d;
        anim9_time = anim9_time_d;
      }
      if(server.argName(0) == "anim10.html"){
        anim10_time = anim10_time_d;
        anim10_V = anim10_V_d;
        anim10_H = anim10_H_d;
        anim10_S = anim10_S_d;
      }
      if(server.argName(0) == "anim11.html"){
        anim11_time = anim11_time_d;
        anim11_H = anim11_H_d;
        anim11_S = anim11_S_d;
        anim11_V = anim11_V_d;
      }
      if(server.argName(0) == "anim12.html"){
        anim12_time = anim12_time_d;
        anim12_H = anim12_H_d;
        anim12_S = anim12_S_d;
        anim12_V = anim12_V_d;
        anim12_time_Up = anim12_time_Up_d;
        anim12_upBy = anim12_upBy_d;
      }
      if(server.argName(0) == "anim13.html"){
        anim13_V = anim13_V_d;
        anim13_move_time = anim13_move_time_d;
      }
      if(server.argName(0) == "anim14.html"){
        anim14_V = anim14_V_d;
        anim14_move_time = anim14_move_time_d;
      }
    }
  }
}
void handle_settings()
{

 // if (server.args() > 0 ) {
 //   for(int i=0;i<server.args();i++){
    //Serial.println(String("Arg name ")+i+": "+server.argName(i));
    //Serial.println(String("Arg value ")+i+": "+server.arg(i));
 //  }
//  }
  if (server.args() > 0 ) { // Arguments were received
     Serial.println("Arguments were received");
    if (server.argName(17)== "save"){
      net_name=server.arg(0);
      if(server.arg(1)!="") pass=server.arg(1);
      ip4=server.arg(2).toInt();
      ip3=server.arg(3).toInt();
      ip2=server.arg(4).toInt();
      ip1=server.arg(5).toInt();
      msk4=server.arg(6).toInt();
      msk3=server.arg(7).toInt();
      msk2=server.arg(8).toInt();
      msk1=server.arg(9).toInt();
      LineCount =server.arg(10).toInt();
      pixelPerLine =server.arg(11).toInt();
      PixelCount = LineCount * pixelPerLine;
      //DEBUG_PRINTLN(PixelCount);
      if (server.arg(12)== "zig") zig = true; 
      if (server.arg(12)== "no_zig") zig = false; 
      if (server.arg(13)== "pir") anim_mode = 1; 
      if (server.arg(13)== "no_pir") anim_mode = 0; 
      if (server.arg(14)== "directlogic") directlogic = true; 
      if (server.arg(14)== "no_directlogic") directlogic = false; 
      cdb =server.arg(15).toInt()*1000;
      foto_rez =server.arg(16).toInt();
      save_eeprom_net(); 
      ip1_s = String(ip1);
      ip2_s = String(ip2);
      ip3_s = String(ip3);
      ip4_s = String(ip4);
      msk1_s = String(msk1);
      msk2_s = String(msk2);
      msk3_s = String(msk3);
      msk4_s = String(msk4);
      //itoa(foto_rez,foto_rez_s,10);
      foto_rez_s = String(foto_rez);
      cdb_s = String(cdb/1000); 
      LineCount_s = String(LineCount);   
      pixelPerLine_s = String(pixelPerLine); 
      net_name.toCharArray(net_name_s,20);
      pass.toCharArray(pass_s,20);   
    }
     
    else if (server.argName(17)== "reset"){
      force_reset();
    }
  }
    
  
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", webpage_1);  
  //server.sendContent(webpage_1);
  server.sendContent(webpage_2);
    if(net_name!="")server.sendContent(net_name);
  server.sendContent(webpage_3+ip4_s);
    //server.sendContent(ip4_s);
  server.sendContent(webpage_4+ip3_s);
    //server.sendContent(ip3_s); 
  server.sendContent(webpage_5+ip2_s);
    //server.sendContent(ip2_s); 
  server.sendContent(webpage_6+ip1_s);
    //server.sendContent(ip1_s); 
  server.sendContent(webpage_7+msk1_s);
    //server.sendContent(msk1_s); 
  server.sendContent(webpage_8+msk2_s);
    //server.sendContent(msk2_s); 
  server.sendContent(webpage_9+msk3_s);
    //server.sendContent(msk3_s); 
  server.sendContent(webpage_10+msk4_s);
    //server.sendContent(msk4_s); 
  server.sendContent(webpage_11+LineCount_s);
    //server.sendContent(LineCount_s); 
  server.sendContent(webpage_12+pixelPerLine_s);
   // server.sendContent(pixelPerLine_s); 
  server.sendContent(webpage_13);
  server.sendContent(webpage_14+cdb_s);
    //server.sendContent(cdb_s); 
  server.sendContent(webpage_15+foto_rez_s);
     //server.sendContent(foto_rez_s); 
  server.sendContent(webpage_16);
   if(zig){
//    Serial.println("zig ");
    server.sendContent("zig");
  }
  else{
//    Serial.println("no zig");
    server.sendContent("no_zig"); 
  }
  server.sendContent(webpage_17);
   if(anim_mode){
//    Serial.println("anim_mode = 0 ");
    server.sendContent("pir");
  }
  else{
//    Serial.println("anim_mode = 1 ");
    server.sendContent("no_pir"); 
  }
  server.sendContent(webpage_18);
  if(directlogic){
//    Serial.println("directlogic = 1 ");
    server.sendContent("directlogic");
  }
  else{
//    Serial.println("directlogic = 0 ");
    server.sendContent("no_directlogic"); 
  }
  server.sendContent(webpage_19);
  server.sendContent(""); 
  server.client().stop(); 
  f_send = true;
  
}
unsigned long last_millis = 0;
void send_rssi()
{
char dbm_c[50];
  unsigned long t = millis();
if((t - last_millis) > 600) {
       // if(f_send) {
          //f_send=false;
          StaticJsonBuffer<200> jsonBuffer;
          JsonObject& json = jsonBuffer.createObject();
          int8_t PdBm_max=-40;
          int8_t PdBm_min=-100;
          int8_t rssi_p = 100 * (1 - (PdBm_max - (float)WiFi.RSSI()) / (PdBm_max - PdBm_min));
          if(rssi_p>100) rssi_p=100;
          if(rssi_p<1) rssi_p=1;
          json["rssi"] = rssi_p;
          json["ilum"] = adc_p;
          json.printTo(dbm_c, sizeof(dbm_c));
          webSocket.sendTXT(0, dbm_c, strlen(dbm_c));
          //}
          //else f_send=true;
          last_millis = millis();
    }
}
void blackScreen();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
//  Serial.printf("webSocketEvent(%d, %d, ...)\r\n", num, type);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();
  boolean f=false;
  switch(type) {
   case WStype_DISCONNECTED:
//    Serial.printf("[%u] Disconnected!\r\n", num);
    c_anim  = String(SerialChar);
    if(SerialChar=='a') c_anim="10";
    if(SerialChar=='c') c_anim="11";
    if(SerialChar=='g') c_anim="12";
    if(SerialChar=='j') c_anim="13";
    if(SerialChar=='k') c_anim="14";
    save_data_ee();
    if(anim_mode==1){
      animations.StopAll();
      blackScreen();   
      canGo=false;
    }
    break;
    case WStype_CONNECTED:
      {
        
        canGo=true;
      }
      break;
    case WStype_TEXT:
    {
//      Serial.printf("[%u] get Text: %s\r\n", num, payload);
      if (strcmp("anim01.html", (const char *)payload) == 0) {  
        SerialChar='1'; 
        data["anim"] = "01";
        data["color"]= "color";
        data[anim1_p[0]]=(int)(anim1_H*360);
        data[anim1_p[1]]= anim1_S;
        data[anim1_p[2]]=(int)(anim1_V*100);
        data[anim1_p[3]]= anim1_deltaH;
        data[anim1_p[4]]=anim1_time;
        c_anim="1";
        f=true;      
//        Serial.print("hue sent:");Serial.println(anim1_H*360);
        }
     if (strcmp("anim02.html", (const char *)payload) == 0) {  
        SerialChar='2'; 
        data["anim"] = "02";
        data[anim2_p[0]]=anim2_time_changeH;
        data[anim2_p[1]]= anim2_time_dimS;
        data[anim2_p[2]]=anim2_time_moveH;
        data[anim2_p[3]]= int(anim2_V*100);
        data[anim2_p[4]]=anim2_fadeBy; 
        c_anim="2";   
        f=true;          
        }
     if (strcmp("anim03.html", (const char *)payload) == 0) {  
        SerialChar='3';  
        data["anim"] = "03";
        data[anim3_p[0]]=anim3_darkenBy;
        data[anim3_p[1]]= anim3_time_scroll;
        data[anim3_p[2]]=anim3_time_fade;
        data[anim3_p[3]]= int(anim3_V*100);
        data[anim3_p[4]]=anim3_deltaH;  
        c_anim="3";  
        f=true;          
        }
     if (strcmp("anim04.html", (const char *)payload) == 0) {
        SerialChar = '4';
        data["anim"]="04";
        data["color"]= "color";        
        data[anim4_p[0]]=int(anim4_H*360);
        data[anim4_p[1]]=anim4_S;
        data[anim4_p[2]]=int(anim4_V*100);
        c_anim="4";  
        f=true; 
        }
     if (strcmp("anim05.html", (const char *)payload) == 0) {  
        SerialChar='5';  
        data["anim"] = "05";
        data[anim5_p[0]]=anim5_t1;
        data[anim5_p[1]]= anim5_t2;
        data[anim5_p[2]]=int(anim5_maxV*100);
        c_anim="5";  
        f=true;          
        }
     if (strcmp("anim06.html", (const char *)payload) == 0) {  
        SerialChar='6';  
        data["anim"] = "06";
        data[anim6_p[0]]= anim6_time_fade;
        data[anim6_p[1]]= anim6_fadeBy;
        data[anim6_p[2]]= anim6_t1;
        data[anim6_p[3]]= anim6_t2;
        data[anim6_p[4]]= anim6_P;
        data[anim6_p[5]]= int(anim6_V*100);
        c_anim="6";  
        f=true;          
        }
      if (strcmp("anim07.html", (const char *)payload) == 0) {  
        SerialChar='7';  
        data["anim"] = "07";
        data[anim7_p[0]]= anim7_time;
        data[anim7_p[1]]= int(anim7_V*100);
        data[anim7_p[2]]= anim7_deltaH;
        c_anim="7";  
        f=true;          
        }
      if (strcmp("anim08.html", (const char *)payload) == 0) {  
        SerialChar='8';  
        data["anim"] = "08";
        data[anim8_p[0]]= anim8_t1;
        data[anim8_p[1]]= anim8_t2;
        data[anim8_p[2]]= int(anim8_V*100);
        c_anim="8";  
        f=true;          
        }
      if (strcmp("anim09.html", (const char *)payload) == 0) {  
        SerialChar='9'; 
        data["anim"] = "09";
        data["color"]= "color";
        data[anim9_p[0]]=(int)(anim9_H*360);
        data[anim9_p[1]]= anim9_S;
        data[anim9_p[2]]=(int)(anim9_V*100);
        data[anim9_p[3]]= anim9_time;
        c_anim="9";
        f=true;      
        }
      if (strcmp("anim10.html", (const char *)payload) == 0) {  
        SerialChar='a'; 
        data["anim"] = "10";
        data["color"]= "color";
        data[anim10_p[0]]=(int)(anim10_H*360);
        data[anim10_p[1]]= anim10_S;
        data[anim10_p[2]]=(int)(anim10_V*100);
        data[anim10_p[3]]=anim10_time;
        c_anim="10";
        f=true;      
        }
      if (strcmp("anim11.html", (const char *)payload) == 0) {  
        SerialChar='c'; 
        data["anim"] = "11";
        data["color"]= "color";
        data[anim11_p[0]]=(int)(anim11_H*360);
        data[anim11_p[1]]= anim11_S;
        data[anim11_p[2]]=(int)(anim11_V*100);
        data[anim11_p[3]]= anim11_time;
        c_anim="11";
        f=true;      
//        Serial.print("hue sent:");Serial.println(anim1_H*360);
        }
      if (strcmp("anim12.html", (const char *)payload) == 0) {  
        SerialChar='g'; 
        data["anim"] = "12";
        data["color"]= "color";
        data[anim12_p[0]]=(int)(anim12_H*360);
        data[anim12_p[1]]= anim12_S;
        data[anim12_p[2]]=(int)(anim12_V*100);
        data[anim12_p[3]]= anim12_time;
        data[anim12_p[4]]= anim12_upBy;
        data[anim12_p[5]]= anim12_time_Up;
        c_anim="12";
        f=true;      
        }
      if (strcmp("anim13.html", (const char *)payload) == 0) {
        SerialChar='j'; 
        data["anim"] = "13";
        data[anim13_p[0]] =  (int)(anim13_V*100);
        data[anim13_p[1]] = anim13_move_time;
        c_anim="13";
        f=true;  
      }
      if (strcmp("anim14.html", (const char *)payload) == 0) {
        SerialChar='k'; 
        data["anim"] = "14";
        data[anim14_p[0]] =  (int)(anim14_V*100);
        data[anim14_p[1]] = anim14_move_time;
        c_anim="14";
        f=true;  
      }
      if(f){
//         Serial.println("json:");
         char buf[300];
         data.printTo(buf);
//         Serial.println("buf: ");
//         Serial.println(buf);
         webSocket.sendTXT(num, buf, strlen(buf));
         timeToChangeAnimation = true;
      }
       
     StaticJsonBuffer<200> jsonBuffer1;
     JsonObject& data1 = jsonBuffer1.parseObject(payload);
      if (!data1.success()) {
       //Serial.println("parseObject() failed");
        return;
        }
//      Serial.println("Rec json:");
//      data.printTo(Serial);

       if (data1["anim"] == "01") {
        anim1_H = data1[anim1_p[0]];
        anim1_H /= 360.0;
        anim1_S = data1[anim1_p[1]];
        anim1_V = data1[anim1_p[2]];
        anim1_V = anim1_V/100.0;
        anim1_deltaH = data1[anim1_p[3]];
        uint16_t aux = data1[anim1_p[4]];
        timeToChangeAnimation = (anim1_time != aux);
        anim1_time = data1[anim1_p[4]];
//        Serial.print("hue get:");Serial.println(anim1_H);
//        Serial.print("anim 1 v:  ");Serial.println(anim1_V);  
        }
      if (data1["anim"] == "02") {
        uint16_t aux = data1[anim2_p[0]];
        timeToChangeAnimation = (anim2_time_changeH != aux);
        aux = data1[anim2_p[1]];
        timeToChangeAnimation |= (anim2_time_dimS != aux);
        aux = data1[anim2_p[2]];
        timeToChangeAnimation |= (anim1_time != aux);
        anim2_time_changeH = data1[anim2_p[0]];
        anim2_time_dimS = data1[anim2_p[1]];
        anim2_time_moveH = data1[anim2_p[2]];
        anim2_V = data1[anim2_p[3]];
        anim2_V /= 100.0;
        anim2_fadeBy = data1[anim2_p[4]];
        }
      if (data1["anim"] == "03") {
        uint16_t aux = data1[anim3_p[1]];
        timeToChangeAnimation = (anim3_time_scroll != aux);
        aux = data1[anim3_p[2]];
        timeToChangeAnimation |= (anim3_time_fade != aux);
        anim3_darkenBy = data1[anim3_p[0]];
        anim3_time_scroll = data1[anim3_p[1]];
        anim3_time_fade = data1[anim3_p[2]];
        anim3_V = data1[anim3_p[3]];
        anim3_V /= 100.0;
        anim3_deltaH = data1[anim3_p[4]];
//        Serial.println("anim 03");
//        Serial.println(anim3_darkenBy);
//        Serial.println(anim3_time_scroll);
//        Serial.println(anim3_time_fade);
//        Serial.println(anim3_V);
        }  
      if (data1["anim"] == "04") {
        anim4_H = data1[anim4_p[0]];
        anim4_H /= 360.0;
        anim4_S = data1[anim4_p[1]];
        anim4_V = data1[anim4_p[2]];
        anim4_V /= 100.0;
        timeToChangeAnimation = true;
//        Serial.println(String("H: ")+anim4_H+" S: "+anim4_S+" V: "+anim4_V);
      }
     if (data1["anim"] == "05") {
        anim5_t1 = data1[anim5_p[0]];
        anim5_t2 = data1[anim5_p[1]];
        anim5_maxV = data1[anim5_p[2]];
        anim5_maxV /= 100.0;
//        Serial.println(String("A5_maxV: ")+anim5_maxV+" 100maxV: "+(anim5_maxV*100.0));
        timeToChangeAnimation = true;
      }  
     if (data1["anim"] == "06") {
        anim6_time_fade = data1[anim6_p[0]];
        anim6_fadeBy = data1[anim6_p[1]];
        anim6_t1 = data1[anim6_p[2]];
        anim6_t2 = data1[anim6_p[3]];
        anim6_P = data1[anim6_p[4]];
        anim6_V = data1[anim6_p[5]];
        anim6_V /= 100.0;
        timeToChangeAnimation = true;
        }  
     if (data1["anim"] == "07") {      
        anim7_time = data1[anim7_p[0]];
        anim7_V = data1[anim7_p[1]];
        anim7_V /= 100.0;
        anim7_deltaH = data1[anim7_p[2]];
        timeToChangeAnimation = true;
        }  
     if (data1["anim"] == "08") {      
        anim8_t1 = data1[anim8_p[0]];
        anim8_t2 = data1[anim8_p[1]];
        anim8_V = data1[anim8_p[2]];
        anim8_V /= 100.0;
        timeToChangeAnimation = true;
        } 
     if (data1["anim"] == "09") {
        anim9_H = data1[anim9_p[0]];
        anim9_H /= 360.0;
        anim9_S = data1[anim9_p[1]];
        anim9_V = data1[anim9_p[2]];
        anim9_V /= 100.0;
        anim9_time = data1[anim9_p[3]];
        timeToChangeAnimation = true;
        }
     if (data1["anim"] == "10") {
        anim10_H = data1[anim10_p[0]];
        anim10_H /= 360.0;
        anim10_S = data1[anim10_p[1]];
        anim10_V = data1[anim10_p[2]];
        anim10_V /= 100.0;
        anim10_time = data1[anim10_p[3]];
        timeToChangeAnimation = true;
        }
     if (data1["anim"] == "11") {
        anim11_H = data1[anim11_p[0]];
        anim11_H /= 360.0;
        anim11_S = data1[anim11_p[1]];
        anim11_V = data1[anim11_p[2]];
        anim11_V /= 100.0;
        anim11_time = data1[anim11_p[3]];
        timeToChangeAnimation = true;
        }
      if (data1["anim"] == "12") {
         anim12_H = data1[anim12_p[0]];
         anim12_H /= 360.0;
         anim12_S = data1[anim12_p[1]];
         anim12_V = data1[anim12_p[2]];
         anim12_V /= 100.0;
         anim12_time = data1[anim12_p[3]];
         anim12_upBy = data1[anim12_p[4]];
         anim12_time_Up = data1[anim12_p[5]];
         timeToChangeAnimation = true; 
        } 
      if (data1["anim"] == "13") {
          anim13_V = data1[anim13_p[0]];
          anim13_V /= 100.0f;
          anim13_move_time = data1[anim13_p[1]];
          timeToChangeAnimation = true; 
        }
      if (data1["anim"] == "14") {
          anim14_V = data1[anim14_p[0]];
          anim14_V /= 100.0f;
          anim14_move_time = data1[anim14_p[1]];
          timeToChangeAnimation = true; 
        }
  }
      break;
    case WStype_BIN:
//      Serial.printf("[%u] get binary length: %u\r\n", num, length);
   //   hexdump(payload, length);
      break;
    default:
//      Serial.printf("Invalid WStype [%d]\r\n", type);
      break;
  }
}

AnimEaseFunction moveEase =
//      NeoEase::Linear;
//      NeoEase::QuadraticInOut;
//      NeoEase::CubicInOut;
//        NeoEase::QuarticInOut;
//      NeoEase::QuinticInOut;
      NeoEase::SinusoidalInOut;
//      NeoEase::ExponentialInOut;
//      NeoEase::CircularInOut;

struct MyAnimationState{
  RgbColor StartingColor;
  RgbColor EndingColor;
}animationState[max_leds];
HsbColor globalColor;


void anim1(const AnimationParam& param){
  float progress = moveEase(param.progress);
  static int16_t count,count_2;
  static HsbColor color;// = HsbColor(anim1_H,anim1_S,anim1_V);
  count = uint16_t(float(LineCount)*progress);
  if(count_2 != count)for(uint16_t i = 0; i<PixelCount;i++)if(i>=count_2*pixelPerLine && i<count*pixelPerLine)strip.SetPixelColor(i,color);
  count_2=count;
  if(param.state == AnimationState_Completed){
    color = HsbColor(anim1_H,anim1_S,anim1_V);
    anim1_H += anim1_deltaH; 
    if(anim1_H>=1.0f)anim1_H=0.0f;
    animations.RestartAnimation(param.index);
  }  
}
void anim2a(const AnimationParam& param){
  float progress = moveEase(param.progress);
  static int16_t c1,c2;
  c1=int16_t(360.0*progress);
  if(c1!=c2){
    for(int i=0;i<PixelCount;i++){
      HsbColor color = strip.GetPixelColor(i);
      //strip.SetPixelColor(i,HsbColor(progress,color.S,anim2_V));  
     strip.SetPixelColor(i,HsbColor(progress,color.S,color.B));  
    }
  }
  c2=c1;
  if(param.state == AnimationState_Completed){
    animations.RestartAnimation(param.index);
  }
}
void anim2c(const AnimationParam& param){
  float progress = moveEase(param.progress);
  static int16_t c1,c2;
  c1=int16_t(float(pixelPerLine)/2.0f*progress);
  int hp = pixelPerLine/2;
  if(c1!=c2){
    for(int i=0;i<LineCount;i++){
     // if(c1>c2+1){
        for(int j=c2;j<=c1;j++){
          HsbColor color = strip.GetPixelColor(i*pixelPerLine+hp+j);
          strip.SetPixelColor(i*pixelPerLine+hp+j,HsbColor(color.H,1.0f,anim2_V));    
          color = strip.GetPixelColor(i*pixelPerLine+hp-j);
          strip.SetPixelColor(i*pixelPerLine+hp-j,HsbColor(color.H,1.0f,anim2_V));   
        }
     // }
      /*else
        {
      HsbColor color = strip.GetPixelColor(i*pixelPerLine+hp+c1);
      strip.SetPixelColor(i*pixelPerLine+hp+c1,HsbColor(color.H,1.0f,color.B));    
      color = strip.GetPixelColor(i*pixelPerLine+hp-c1);
      strip.SetPixelColor(i*pixelPerLine+hp-c1,HsbColor(color.H,1.0f,color.B));   
    }*/
  }
  }
  c2=c1;
  if(param.state==AnimationState_Completed){
    animations.RestartAnimation(param.index);
  }
}
void anim2b_2(float fadeBy){
  HsbColor color;
  for(uint16_t i=0;i<PixelCount;i++){
    color = strip.GetPixelColor(i);
    strip.SetPixelColor(i,HsbColor(color.H,color.S*fadeBy,color.B*fadeBy*0.9));    
  }
}
void anim2b_3(const AnimationParam& param){
  if(param.state==AnimationState_Completed){
    anim2b_2(anim2_fadeBy);
    animations.RestartAnimation(param.index);
  }
}
void anim3a(const AnimationParam& param){
  float progress = moveEase(param.progress);
  uint16_t nextLine;
  static uint16_t lastLine=0;
  static float H=0.0f;
  static HsbColor CylonEyeColor = HsbColor(H,1.0f,anim3_V);
  static int moveDir = 1;
  if(moveDir>0)nextLine = uint16_t(float(LineCount)*progress);else nextLine = uint16_t(float(LineCount)*(1.0f-progress));
  
  if(lastLine != nextLine){
    if(lastLine<nextLine){
      for(uint16_t i = 0; i<PixelCount;i++)if(i>=lastLine*pixelPerLine && i<nextLine*pixelPerLine)strip.SetPixelColor(i,CylonEyeColor);
    }else{
      for(uint16_t i = 0; i<PixelCount;i++)if(i>=nextLine*pixelPerLine && i<lastLine*pixelPerLine)strip.SetPixelColor(i,CylonEyeColor);
    }
    }
  lastLine=nextLine;
  if(param.state == AnimationState_Completed){
    H+=anim3_deltaH;
    if(H>=1.0f)H=0.0f;
    CylonEyeColor = HsbColor(H,1.0f,anim3_V);
    moveDir *= -1;
    animations.RestartAnimation(param.index);
  }
}
void anim3b_1(uint8_t darkenBy){
  RgbColor color;
  for (uint16_t indexPixel = 0; indexPixel < PixelCount; indexPixel++){
        color = strip.GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}
void anim3b_2(const AnimationParam& param){
    if (param.state == AnimationState_Completed){
        anim3b_1(anim3_darkenBy);
        animations.RestartAnimation(param.index);
    }
}
void anim4(HsbColor color){ 
  for(uint16_t i=0;i<PixelCount;i++)strip.SetPixelColor(i,color);
}

void anim5a(const AnimationParam& param){
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor,animationState[param.index].EndingColor,param.progress);
  for(int i=0;i<pixelPerLine;i++)strip.SetPixelColor(param.index*pixelPerLine+i,updatedColor);
}
void anim5b(int idx, float luminance){
    int t = random(anim5_t1,anim5_t2);
    animationState[idx].StartingColor=strip.GetPixelColor(idx*pixelPerLine);
    animationState[idx].EndingColor=HsbColor(random(360)/360.0f,1.0f,luminance);
    animations.StartAnimation(idx,t,anim5a);
}
void anim6a(const AnimationParam& param){
  float progress = moveEase(param.progress);
  static int c1,c2=0;
  static int dir = 1;
  static boolean p=false;
  if(dir == 1)c1=int(float(pixelPerLine) * progress); else c1 = int(float(pixelPerLine)*(1-progress));
  if(c1!=c2){
    if(dir>0){
      if(p)for(int i=c2;i<=c1;i++)strip.SetPixelColor(param.index*pixelPerLine+i,animationState[param.index].StartingColor); 
      else strip.SetPixelColor(param.index*pixelPerLine+c1,animationState[param.index].StartingColor); 
    }else{
      if(p)for(int i=c2;i>=c1;i--)strip.SetPixelColor(param.index*pixelPerLine-i,animationState[param.index].StartingColor); 
      else strip.SetPixelColor(param.index*pixelPerLine-c1,animationState[param.index].StartingColor);
    }
  }
  c2=c1;
  if(param.state == AnimationState_Completed){
    int aux = random(10);
    if(aux>5)dir = -dir;
    aux = random(100);
    if(aux>anim6_P)p=true;else p=false;
  }
}
void anim6b(int idx, float luminance){
  int t=random(anim6_t1,anim6_t2);
  animationState[idx].StartingColor=HsbColor(random(360)/360.0f,random(300)/300.0f,luminance);
  animations.StartAnimation(idx,t,anim6a);
}
void anim6c(const AnimationParam& param){
    if (param.state == AnimationState_Completed){
        anim3b_1(anim6_fadeBy);
        animations.RestartAnimation(param.index);
    }
}
void anim7a(const AnimationParam& param){
  static int dir = 1;
  float progress = moveEase(param.progress);
  static HsbColor color = HsbColor(0.0f,1.0f,0.06f);
  if(dir>0) color = HsbColor(color.H,color.S,progress*anim7_V);else color = HsbColor(color.H,color.S,anim7_V*(1.0f-progress));
  for(int i=0;i<PixelCount;i++)strip.SetPixelColor(i,color);
  if(param.state==AnimationState_Completed){
    color.H+=anim7_deltaH;
    if(color.H>1.0f)color.H-=1.0f;
    dir = -dir;
    animations.RestartAnimation(param.index);
  }
}
void anim8a(const AnimationParam& param){
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor,animationState[param.index].EndingColor,param.progress);
  strip.SetPixelColor(param.index,updatedColor);
}
void anim8b(float luminance){
  for(uint16_t i=0;i<PixelCount;i++){
    uint16_t time = random(anim8_t1,anim8_t2);
    animationState[i].StartingColor = strip.GetPixelColor(i);
    animationState[i].EndingColor = HsbColor(random(360)/360.0f,1.0f,luminance);
    animations.StartAnimation(i,time,anim8a);
  }
}
void anim8c(float luminance){
  if(animations.IsAnimating()){
    animations.UpdateAnimations();
    strip.Show();
  }else anim8b(luminance);
}
void anim9(HsbColor color){
  //
  //DEBUG_PRINTLN(PixelCount);
  for(int i=0;i<PixelCount;i++)strip.SetPixelColor(i,RgbColor(0,0,0));
  for(uint16_t i = 0;i<pixelPerLine;i++){
    strip.SetPixelColor(i,color);
    strip.SetPixelColor((LineCount-1)*pixelPerLine+i,color);    
  }
}
void anim9b(const AnimationParam& param){
  for(int i=1;i<LineCount-1;i++)for(int j=0;j<pixelPerLine;j++)strip.SetPixelColor(i*pixelPerLine+j,HsbColor(anim9_H, anim9_S, anim9_V*param.progress));
}
void anim9c(const AnimationParam& param){
  for(int i=1;i<LineCount-1;i++)for(int j=0;j<pixelPerLine;j++)strip.SetPixelColor(i*pixelPerLine+j,HsbColor(anim9_H, anim9_S, anim9_V*(1.0f-param.progress)));
}
void anim10a(HsbColor color, int currentLine){
  for(uint16_t i=0;i<pixelPerLine;i++)strip.SetPixelColor(currentLine*pixelPerLine+i,color);
}
void anim10b(const AnimationParam& param){
  float incT_p=anim10_deltaT_p - anim10_tau_p;
  float H[LineCount];
  uint16_t linenumber = uint16_t(param.progress/incT_p);
  
  for(uint16_t i = 0;i<LineCount;i++){
    if(i<=linenumber){
      H[i]=constrain(anim10_V*(param.progress-float(i)*anim10_deltaT_p)/anim10_deltaT_p,0.0f,anim10_V);
      anim10a(HsbColor(anim10_H,anim10_S,H[i]),i);
    }
  }
}
void anim10c(const AnimationParam& param){
  float incT_p = anim10_deltaT_p - anim10_tau_p;
  float H[LineCount];
  uint16_t linenumber = uint16_t(param.progress/incT_p);
  
  for(uint16_t i = 0;i<LineCount;i++){
    if(i<=linenumber){
      H[i]=constrain(anim10_V - anim10_V*(param.progress-float(i)*anim10_deltaT_p)/anim10_deltaT_p,0.0f,anim10_V);
      anim10a(HsbColor(anim10_H,anim10_S,H[i]),i);
    }
  }
}
void anim10d(const AnimationParam& param){
  float incT_p=anim10_deltaT_p - anim10_tau_p;
  float H[LineCount];
  uint16_t linenumber = uint16_t(param.progress/incT_p);
  
  for(uint16_t i = 0;i<LineCount;i++){
    if(i<=linenumber){
      H[i]=constrain(anim10_V*(param.progress-float(i)*anim10_deltaT_p)/anim10_deltaT_p,0.0f,anim10_V);
      anim10a(HsbColor(anim10_H,anim10_S,H[i]),LineCount-i-1);
    }
  }
}
void anim10e(const AnimationParam& param){
  float incT_p = anim10_deltaT_p - anim10_tau_p;
  float H[LineCount];
  uint16_t linenumber = uint16_t((1.0f-param.progress)/incT_p);
  
  for(int i = 0;i<LineCount;i++){
    if(i>=linenumber){
      H[i]=constrain(anim10_V - anim10_V*(param.progress-float(i)*anim10_deltaT_p)/anim10_deltaT_p,0.0f,anim10_V);
      anim10a(HsbColor(anim10_H,anim10_S,H[i]),i);
    }
  }
}
void anim11a(uint16_t x, uint16_t y, uint16_t h, uint16_t l, HsbColor color){
  float xf=x,yf=y,hf=h,lf=l,alfa=atan(hf*2.0f/lf);
  uint16_t pVf=x+pixelPerLine*y;
  for(uint16_t i=0;i<pixelPerLine;i++){
    for(uint16_t j=0;j<LineCount;j++){
      uint16_t aux = i;
      if(j%2)aux = pixelPerLine-i-1;//////////zing-zag comment for not
      if(!zig) aux = i;
      if((j>=y) && (j<=h+y) && (i>x-(j-y)/tan(alfa)) && (i<x+(j-y)/tan(alfa)))strip.SetPixelColor(aux+pixelPerLine*j,color);//else strip.SetPixelColor(i+pixelPerLine*j+aux,RgbColor(0,0,0));
    }
  }
}
void anim11a_2(uint16_t x, uint16_t y, uint16_t h, uint16_t l, HsbColor color){
  float xf=x,yf=y,hf=h,lf=l,alfa=atan(hf*2.0f/lf);
  uint16_t pVf=x+pixelPerLine*y;
  for(uint16_t i=0;i<pixelPerLine;i++){
    for(uint16_t j=0;j<LineCount;j++){
      uint16_t aux = i;
      if(j%2)aux = pixelPerLine-i-1;//////////zing-zag comment for not
      if(!zig)aux = i;
      if((j<=y) && (j>=y-h) && (i>x+(j-y)/tan(alfa)) && (i<x-(j-y)/tan(alfa)))strip.SetPixelColor(aux+pixelPerLine*j,color);
    }
  }
}
void anim11b(const AnimationParam& param){
  HsbColor color(anim11_H, anim11_S, anim11_V);
  for(uint16_t i=0;i<PixelCount;i++)strip.SetPixelColor(i,HsbColor(anim4_H,anim4_S,anim4_V));
  anim11a(pixelPerLine/2,uint16_t(float(LineCount)*(1.0f-param.progress)),8,24,color);
//  anim11a(pixelPerLine/4,uint16_t(float(LineCount+5)*(1.0f-param.progress)),6,10,color);
//  anim11a(pixelPerLine*3/4,uint16_t(float(LineCount+5)*(1.0f-param.progress)),6,10,color);
  if(param.state == AnimationState_Completed){
    animations.RestartAnimation(param.index);
  }
}
void anim11c(const AnimationParam& param){
  HsbColor color(anim11_H, anim11_S, anim11_V);
  for(uint16_t i=0;i<PixelCount;i++)strip.SetPixelColor(i,HsbColor(anim4_H,anim4_S,anim4_V));
  anim11a_2(pixelPerLine/2,uint16_t(float(LineCount)*(param.progress)),8,24,color);
//  anim11a_2(pixelPerLine/4,uint16_t(float(LineCount+5)*(param.progress)),6,10,color);
//  anim11a_2(pixelPerLine*3/4,uint16_t(float(LineCount+5)*(param.progress)),6,10,color);
  if(param.state == AnimationState_Completed){
    animations.RestartAnimation(param.index);
  }
}
void anim12a(float upBy){
  HsbColor color;
  for(int i=0;i<PixelCount;i++){
    color = strip.GetPixelColor(i);
    //color.B = color.B * upBy;
    if(color.B!=0.0f){
      float ax = 0.005+(anim12_V*upBy/500.0f);
      color.B = color.B + ax;
    }
    if(color.B > anim12_V) color.B = anim12_V;
    strip.SetPixelColor(i,color);
  }
}
void anim12b(const AnimationParam& param){
  if(param.state==AnimationState_Completed){
    anim12a(anim12_upBy);
    animations.RestartAnimation(param.index);
  }
}
void anim12b_2(const AnimationParam& param){
  if(param.state == AnimationState_Completed){
    anim12a(1.0f/anim12_upBy);
    animations.RestartAnimation(param.index);
  }
}
void anim12c(const AnimationParam& param){
  static int op;
  int cp = int(float(PixelCount)*param.progress);
  if(op!=cp){
    for(int i=op;i<cp;i++){
      HsbColor color = strip.GetPixelColor(i);
      color.B=0.01;
      color.H=anim12_H;
      color.S=anim12_S;
      strip.SetPixelColor(i,color);  
    }
    op=cp;
  }
}
void anim12c_2(const AnimationParam& param){
  static int op=PixelCount-1;
  int cp = int(float(PixelCount) * (1.0f - param.progress));
  if(op!=cp){
    for(int i=op;i>cp;i--){
      HsbColor color = strip.GetPixelColor(i);
      color.B=0.01;
      color.H=anim12_H;
      color.S=anim12_S;
      strip.SetPixelColor(i,color);  
    }
    op=cp;
  }
}
void anim12c_s(const AnimationParam& param){
  static int op;
  int cp = int(float(PixelCount)*param.progress);
  if(op!=cp){
    for(int i=op;i<cp;i++){
      HsbColor color = strip.GetPixelColor(i);
      color.B=anim12_V;
      color.H=anim12_H;
      color.S=anim12_S;
      strip.SetPixelColor(i,color);  
    }
    op=cp;
  }
}
void anim12c_2_s(const AnimationParam& param){
  static int op=PixelCount-1;
  int cp = int(float(PixelCount) * (1.0f - param.progress));
  if(op!=cp){
    for(int i=op;i>cp;i--){
      HsbColor color = strip.GetPixelColor(i);
      color.B=anim12_V;
      color.H=anim12_H;
      color.S=anim12_S;
      strip.SetPixelColor(i,color);  
    }
    op=cp;
  }
}
void anim13(const AnimationParam& param){
static int xPos,yPos,dirX = 1,dirY = 1;
R.B = anim13_V;
Y.B = anim13_V;
B.B = anim13_V;
 if(param.state == AnimationState_Completed){
    xPos += dirX;
    yPos += dirY;
    if(xPos < 0){dirX = 1;xPos = 1;}
    if(yPos < 0){dirY = 1;yPos = 1;}
    if(xPos + steagW > pixelPerLine){dirX = -1; xPos = pixelPerLine - steagW - 1;}
    if(yPos + steagH > LineCount){dirY = -1; yPos = LineCount - steagH - 1;}
    for(int i=0;i<pixelPerLine;i++){
      for(int j=0;j<LineCount;j++){
        int aux = i;
        if(j%2)aux = pixelPerLine - i - 1;
        if(!zig)aux = i;
        strip.SetPixelColor(aux + pixelPerLine * j,HsbColor(0.0f,0.0f,0.0f));
        if(j >= yPos && j < yPos + steagH){
          if(i >= xPos && i < xPos + steagW_1)strip.SetPixelColor(aux + pixelPerLine * j,B);
          if(i >= xPos + steagW_1 && i < xPos + 2 * steagW_1)strip.SetPixelColor(aux + pixelPerLine * j,Y);
          if(i >= xPos + 2 * steagW_1 && i < xPos + 3 * steagW_1)strip.SetPixelColor(aux + pixelPerLine * j,R);
          //Serial.print(aux + pixelPerLine * j);Serial.print(" ");
        }
      }
    }
    animations.RestartAnimation(param.index);
  }
}

void anim14(const AnimationParam& param){
  static boolean dir=true;
 float progress = NeoEase::SinusoidalInOut(param.progress);
   if(!dir) progress = 1-progress; 
  R.B = anim14_V*progress;
  Y.B = R.B;
  B.B = R.B;
  for(int i=0;i<pixelPerLine;i++){
    for(int j=0;j<LineCount;j++){
      int aux = i;
      if(j%2)aux = pixelPerLine - i - 1;
      if(!zig)aux = i;
      if(i >= 0 && i < pixelPerLine/3)strip.SetPixelColor(aux + pixelPerLine * j,B);
      if(i >= pixelPerLine/3 && i < 2*pixelPerLine/3)strip.SetPixelColor(aux + pixelPerLine * j,Y);
      if(i >= 2*pixelPerLine/3 && i < pixelPerLine)strip.SetPixelColor(aux + pixelPerLine * j,R);
      }
    }
 if(param.state == AnimationState_Completed){
    dir=!dir;
    animations.RestartAnimation(param.index);
  }
}

void testL(float lum){
  for(uint16_t i=0;i<PixelCount;i++)strip.SetPixelColor(i,HsbColor(0.0f,1.0f,lum));
}

void blackScreen(){
  for(uint16_t i=0;i<PixelCount;i++)strip.SetPixelColor(i,RgbColor(0,0,0));
}
void makeItHappen(char c){
  switch(c){
    case '1':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim1_time,anim1);
//      Serial.println(String("anim1_time: ")+anim1_time);
      break;
    case '2':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim2_time_moveH,anim2c);
      animations.StartAnimation(1,anim2_time_changeH,anim2a);
      animations.StartAnimation(2,anim2_time_dimS,anim2b_3);
      
      break;
    case '3':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim3_time_scroll,anim3a);
      animations.StartAnimation(1,anim3_time_fade,anim3b_2);
      break;
    case '4':
      animations.StopAll();
      blackScreen();//new
      anim4(HsbColor(anim4_H, anim4_S, anim4_V));
      break;
    case '5':
      animations.StopAll();
      blackScreen();//new
      break;
    case'6':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(LineCount,anim6_time_fade,anim6c);
      break;
    case'7':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(7,anim7_time,anim7a);
      break;
    case'8':
      animations.StopAll();
      blackScreen();//new
      break;
    case '9':
      animations.StopAll();
      blackScreen();//new
      anim9(HsbColor(anim9_H,anim9_S,anim9_V));
      break;
    case 'a':
      animations.StopAll();
      blackScreen();
      animations.StartAnimation(0,anim10_time,anim10b);
      break;
    case 'b':
      animations.StopAll();
   //   blackScreen();//new
      animations.StartAnimation(0,anim10_time,anim10c);
      break;
    case 'd':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim11_time,anim11b);
      //anim11a(32,8,10,16,HsbColor(0.0f,1.0f,0.07f));
      break;
    case 'c':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim11_time,anim11c);
      //anim11a_2(20,12,10,16,HsbColor(0.0f,1.0f,0.07f));
      break;
    case 'e':
      animations.StopAll();
      blackScreen();
      animations.StartAnimation(0,anim10_time,anim10d);
      break;
    case 'f':
      animations.StopAll();
 //     blackScreen();//new
      animations.StartAnimation(0,anim10_time,anim10e);
      break;
    case 'l':
      animations.StopAll();
      blackScreen();
      animations.StartAnimation(0,anim12_time_Up,anim12b);
      animations.StartAnimation(1,anim12_time,anim12c);
      break;
    case 'h':
      animations.StopAll();
      animations.StartAnimation(0,anim9_time,anim9b);
      break;
    case 'i':
      animations.StopAll();
      //blackScreen();//new
      animations.StartAnimation(0,anim9_time,anim9c);
      SerialChar = '9';
      break;
    case'j':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim13_move_time,anim13);
      strip.Show();
      break;  
    case'k':
      animations.StopAll();
      blackScreen();//new
      animations.StartAnimation(0,anim14_move_time,anim14);
      strip.Show();
      break;    
    case 'g':
      animations.StopAll();
      blackScreen();
      animations.StartAnimation(0,anim12_time_Up,anim12b);
      animations.StartAnimation(1,anim12_time,anim12c_2);
      break;
    case 'm':
      animations.StopAll();
      animations.StartAnimation(0,anim12_time_Up,anim12b_2);
      animations.StartAnimation(1,anim12_time,anim12c_s);
      break;
    case 'n':
      animations.StopAll();
      animations.StartAnimation(0,anim12_time_Up,anim12b_2);
      animations.StartAnimation(1,anim12_time,anim12c_2_s);
      break;
    case'0':
      animations.StopAll();
      for(int i=0;i<PixelCount;i++)strip.SetPixelColor(i,RgbColor(0,0,0));
      strip.Show();
      break;   
  
  }
}

// upa,downa cda = 15000, cdb = 5000;

void check_buttons(){
  static boolean hasStarted=false,hasStopped=false,anim_mode_aux=false,foto_trig=false,lastanim9=false;
  static unsigned long cTime = millis(),lastActivity;
  if(millis()-cTime<500)return;
  cTime=millis();

  if(anim_mode==1){
    anim_mode_aux=false;
    if(SerialChar == '9' && adc_p < foto_rez){anim9(HsbColor(anim9_H,anim9_S,anim9_V));lastanim9=true;}
    if(lastanim9 && adc_p >= foto_rez){
      blackScreen();
      lastanim9=false;
    }
    if(!hasStarted){
      if(!hasStopped){
        if(SerialChar == 'a' || SerialChar == 'e'){
          int x = random(100);
          if(x>50)SerialChar='f';else SerialChar='b';
          timeToChangeAnimation=true;
          hasStopped=true;
          canGo=false;
          return;
        }
        if(SerialChar == 'g'){
          SerialChar='m';
          timeToChangeAnimation=true;
          hasStopped=true;
          canGo=false;
          return;
        }
        if(SerialChar == 'l'){
          SerialChar='n';
          timeToChangeAnimation=true;
          hasStopped=true;
          canGo=false;
          return;
        }
        if(SerialChar == 'h'){
          SerialChar = 'i';
          timeToChangeAnimation=true;
          hasStopped=true;
          canGo=false;
          return;
        }
        hasStopped=true;
        canGo=false;
        animations.StopAll();
        blackScreen();
        timeToChangeAnimation=false;
        if(SerialChar == '9'){anim9(HsbColor(anim9_H,anim9_S,anim9_V));lastanim9=true;}
        if(SerialChar == '9' && adc_p >= foto_rez)blackScreen();
      }
    }
    //***************************** BUTOANE ***************************************//
    if((digitalRead(pir_up)== !directlogic || digitalRead(pir_down)==!directlogic) && adc_p < foto_rez){
      lastActivity = millis();
      canGo=true;
      hasStopped=false;
      if(!hasStarted){
        hasStarted=true;
        if(digitalRead(pir_down)== !directlogic ){
          if(SerialChar == 'd')SerialChar='c';
          if(SerialChar == 'e' || SerialChar == 'b' || SerialChar == 'f')SerialChar='a';
          if(SerialChar == 'g')SerialChar='l';
        }
        if(digitalRead(pir_up)== !directlogic ){
          if(SerialChar == 'c')SerialChar='d';
          if(SerialChar == 'a' || SerialChar == 'b' || SerialChar == 'f')SerialChar='e';
          if(SerialChar == 'l')SerialChar='g';
        }
        if(SerialChar == '9') {
          SerialChar = 'h';
          lastanim9 = true;
        }
        timeToChangeAnimation=true;
      }
    }
    if(digitalRead(pir_up)== directlogic  && digitalRead(pir_down)== directlogic )if(millis()-lastActivity>cdb)hasStarted=false;
  }
  if(anim_mode==0){
    if(adc_p < foto_rez - foto_hys && !foto_trig){
      anim_mode_aux = false;
      foto_trig = true;
    }
    if(adc_p > foto_rez + foto_hys){
      foto_trig = false;
      anim_mode_aux = true;
      animations.StopAll();
      for(int i=0;i<PixelCount;i++)strip.SetPixelColor(i,RgbColor(0,0,0));
      strip.Show();
    }
    if(!anim_mode_aux){
      hasStopped=false;
      hasStarted=false;
      canGo=true;
      anim_mode_aux=true;
      timeToChangeAnimation=true;
    }
  }
}

void read_adc(){
  unsigned long currentMillis_adc = millis();

  if (currentMillis_adc - previousMillis_adc >= 500) {
    // save the last time you blinked the LED
    previousMillis_adc = currentMillis_adc;
    int adc_raw = analogRead(adc_pin);
    //Serial.print("analogRead:");Serial.println(adc_raw);
    //adc = adc_raw*0.1 + adc*0.9;
    adc = adc + (adc_raw-adc)*0.4;
    //Serial.print("adc:");Serial.println(adc);    
    adc_p = 100 * (1 - (512 - (float)adc) / (512));
    //Serial.print("adc%:");Serial.print(adc_p);Serial.println("%"); 
    } 
}

bool reset_btn(){
  current = digitalRead(rst_pin);
  if (current == LOW && previous == HIGH && (millis() - firstTime) > 200) {
    firstTime = millis();
  }
  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;
  if (millis_held > 50) {
    if (current == HIGH && previous == LOW) {
      if (secs_held >= 3) return true;
    }
  previous = current;
  prev_secs_held = secs_held;
  }
return false;
}
void led_blink(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    if(WiFi.getMode()==WIFI_AP){
      digitalWrite(led2,HIGH);
      digitalWrite(led1, ledState);
     }
    if(WiFi.getMode()==WIFI_STA){
      digitalWrite(led1,HIGH);
      digitalWrite(led2, ledState); 
    }
  }
}



void setup(){
    Serial.begin(115200);
    pinMode(pir_down,INPUT_PULLUP);
    pinMode(pir_up,INPUT_PULLUP);
    pinMode(rst_pin, INPUT_PULLUP); 
    pinMode(led1, OUTPUT); 
    pinMode(led2, OUTPUT); 

    adcAttachPin(adc_pin);
    adcStart(adc_pin);
    analogReadResolution(9); // Default of 12 is not very linear. Recommended to use 10 or 11 depending on needed resolution.
    analogSetAttenuation(ADC_6db); // max 1.9V Default is 11db which is very noisy. Recommended to use 2.5 or 6.
    analogSetSamples(1);  
    
    delay(1000);
    EEPROM.begin(10);
    byte b = EEPROM.read(0);
    EEPROM.end();
    if(b){
      load_eeprom_net();
      load_eeprom_data();
    }
    load_eeprom_data();
    strip.Begin();
    for(int i=0;i<strip.PixelCount();i++)strip.SetPixelColor(i,RgbColor(0,0,0));
    strip.Show();
    delay(1000);

    EEPROM.begin(10);
    b = EEPROM.read(0);
    EEPROM.end();
    if(!b){
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
      String s = "ESP_"+WiFi.macAddress();
      WiFi.softAP(s.c_str());//  string to (*char)
      Serial.print(F("Connect to "));Serial.print(s);Serial.println(F(" access point"));
      String IPaddress = apIP.toString();
      Serial.print(F("Go to "));Serial.print(IPaddress);Serial.println(F(" address in browser"));
    }else{
      WiFi.mode(WIFI_STA);
      local_ip[0]=ip4;local_ip[1]=ip3;local_ip[2]=ip2;local_ip[3]=ip1;subnet_ip[0]=msk4;subnet_ip[1]=msk3;subnet_ip[2]=msk2;subnet_ip[3]=msk1;gateway_ip=local_ip;gateway_ip[3]=1;
      Serial.println(String("Router: ")+net_name+" "+pass+" "+local_ip+" "+gateway_ip+" "+subnet_ip);
      WiFi.begin(net_name.c_str(), pass.c_str());
      WiFi.config(local_ip, gateway_ip, subnet_ip);
      int i=0;boolean t=false;
      while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
        if(++i>=200){
          t=true;
          EEPROM.begin(10);
          EEPROM.put(0,0);
          EEPROM.end();
          break;
        }
      }
    
      Serial.println("");
      if(!t){
        Serial.print("Connected to ");
        Serial.println(net_name);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
      }else{
        Serial.println("Failed to connect to router!");
        WiFi.mode(WIFI_AP);
        WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
        String s = "ESP_"+WiFi.macAddress();
        WiFi.softAP(s.c_str());//  string to (*char)
        Serial.print(F("Connect to "));Serial.print(s);Serial.println(F(" access point"));
        String IPaddress = apIP.toString();
        Serial.print(F("Go to "));Serial.print(IPaddress);Serial.println(F(" address in browser"));
      }
    }
    server.on("/", handleRoot);
    webSocket.begin();
    server.on("/anim01.html", handle_anim);
    server.on("/anim02.html", handle_anim);
    server.on("/anim03.html", handle_anim);
    server.on("/anim04.html", handle_anim);
    server.on("/anim05.html", handle_anim);
    server.on("/anim06.html", handle_anim);
    server.on("/anim07.html", handle_anim);
    server.on("/anim08.html", handle_anim);
    server.on("/anim09.html", handle_anim);
    server.on("/anim10.html", handle_anim);
    server.on("/anim11.html", handle_anim);
    server.on("/anim12.html", handle_anim);
    server.on("/anim13.html", handle_anim);
    server.on("/anim14.html", handle_anim);
    server.on("/settings.html", handle_settings);
    server.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop(){
  if(SerialChar=='5' && canGo)for(int i=0;i<LineCount;i++)if(!animations.IsAnimationActive(i))anim5b(i, random(int(2000.0f*float(anim5_maxV)))/2000.0f); 
  if(SerialChar=='6' && canGo)for(int i=0;i<LineCount;i++)if(!animations.IsAnimationActive(i))(anim6b(i,anim6_V));
  if(SerialChar=='8' && canGo)anim8c(anim8_V);else{
    //taskDISABLE_INTERRUPTS();
    animations.UpdateAnimations();
    //taskENABLE_INTERRUPTS();
    strip.Show();  
    
  }  

  
  if(Serial.available()){
    SerialChar=Serial.read();
    timeToChangeAnimation=true;
  }
  if(timeToChangeAnimation){
    makeItHappen(SerialChar);  
    timeToChangeAnimation=false;
  }
    
 check_buttons();
 webSocket.loop();
 //led_blink();
 read_adc();
 server.handleClient();
 if(f_send) send_rssi();
 if(reset_btn()){
   Serial.println(F("Button held for 3s"));
   EEPROM.begin(128);
   EEPROM.put(0,0);
   EEPROM.end();
   force_reset();
  }

}
