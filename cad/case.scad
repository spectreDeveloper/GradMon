/*
    💎 GradMon Case v2.0
    Minimal, support-free stand for PSA/BGS graded cards.
    Optimized for Heltec WiFi Kit 32 V3 with SIDE USB-C port.
*/

// --- SETTINGS ---
$fn = 64;

// Card dimensions (Slabs)
card_width = 83.5;
card_depth = 9.0;    
card_height_vis = 25; 

// ESP32 Module (Heltec WiFi Kit 32 V3)
esp_w = 51.0;
esp_d = 26.5;
esp_h = 11.0;
oled_w = 26.0;
oled_h = 14.0;

// USB Plug space (Side entry)
usb_plug_w = 20; // Space for the connector body
usb_plug_h = 12;

// Base dimensions
base_w = 105; // Slightly wider for USB plug clearance
base_d = 65;
base_h = 38;
angle = 15; 

module case() {
    difference() {
        // --- 1. EXTERNAL SHELL ---
        union() {
            hull() {
                // Front
                translate([-base_w/2, 0, 0]) cube([base_w, 2, 28]);
                // Back
                translate([-base_w/2, base_d-2, 0]) cube([base_w, 2, base_h]);
                // Bottom
                translate([-base_w/2, 0, 0]) cube([base_w, base_d, 2]);
            }
        }

        // --- 2. CARD SLOT ---
        translate([0, base_d/2 + 8, base_h - card_height_vis + 10])
        rotate([-angle, 0, 0])
        translate([-card_width/2, -card_depth/2, -10])
        cube([card_width, card_depth, 100]);

        // --- 3. ESP32 CAVITY & OLED WINDOW ---
        // Cavity for the board (centered)
        translate([0, 10, 15])
        cube([esp_w + 2, 30, esp_h + 2], center=true);
        
        // OLED Window (Front)
        translate([0, -5, 15])
        cube([oled_w, 20, oled_h], center=true);

        // --- 4. USB SIDE SPACE & REAR EXIT ---
        // Extra space on the side for the USB-C plug
        // Assuming the port is on the "left" end of the module
        translate([-(esp_w/2 + usb_plug_w/2), 10, 15])
        cube([usb_plug_w, 15, usb_plug_h], center=true);
        
        // Channel from side space to the center
        translate([-base_w/4, 10, 15])
        cube([base_w/2, 10, usb_plug_h], center=true);

        // Exit hole to the back
        translate([0, base_d, 12])
        rotate([90, 0, 0])
        hull() {
            cylinder(d=12, h=base_d);
            translate([0, -20, 0]) cylinder(d=12, h=base_d);
        }
        
        // Internal hollow to clear the cable path
        translate([0, base_d/2 + 5, 12])
        cube([base_w - 20, 15, 15], center=true);
    }
}

case();

// --- PRINTING INFO ---
/*
    - Support: NOT REQUIRED
    - Orientation: Print on bottom face
    - Infill: 15-20%
    - Material: PLA/PETG
*/
