/*
    💎 GradMon Case v3.0
    Minimal, support-free stand for PSA/BGS graded cards.
    Features an open "Side Corridor" for easy USB cable management.
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

// USB Corridor dimensions
corridor_w = 16; 
corridor_h = 14;

// Base dimensions
base_w = 105;
base_d = 65;
base_h = 38;
angle = 15; 

module case() {
    difference() {
        // --- 1. EXTERNAL SHELL ---
        union() {
            hull() {
                // Front slanted face
                translate([-base_w/2, 0, 0]) cube([base_w, 2, 28]);
                // Back face
                translate([-base_w/2, base_d-2, 0]) cube([base_w, 2, base_h]);
                // Bottom plate
                translate([-base_w/2, 0, 0]) cube([base_w, base_d, 2]);
            }
        }

        // --- 2. CARD SLOT ---
        // Slanted slot to hold the slab
        translate([0, base_d/2 + 10, base_h - card_height_vis + 10])
        rotate([-angle, 0, 0])
        translate([-card_width/2, -card_depth/2, -10])
        cube([card_width, card_depth, 100]);

        // --- 3. ESP32 CAVITY & OLED WINDOW ---
        // Main cavity for the board
        translate([0, 12, 15])
        cube([esp_w + 2, 25, esp_h + 2], center=true);
        
        // Window for the OLED screen
        translate([0, -5, 15])
        cube([oled_w, 20, oled_h], center=true);

        // --- 4. USB "CORRIDOR" ---
        // Cutout from the side port (left) straight to the back
        hull() {
            // Starting at the port location (left end of board)
            translate([-esp_w/2 - corridor_w/4, 12, 15])
            cube([corridor_w, 15, corridor_h], center=true);
            
            // Ending at the rear face
            translate([-esp_w/2 - corridor_w/4, base_d, 15])
            cube([corridor_w, 1, corridor_h], center=true);
        }
        
        // Extra clearance for the cable bend at the back
        translate([-base_w/4, base_d-5, 15])
        cube([base_w/2, 15, corridor_h], center=true);
    }
}

case();

// --- PRINTING INFO ---
/*
    - Support: NOT REQUIRED
    - Orientation: Print on bottom face (as oriented in OpenSCAD)
    - Infill: 15%
    - Material: PLA is perfect.
*/
