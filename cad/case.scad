/*
    💎 GradMon Case
    Minimal, support-free stand for PSA/BGS graded cards and Heltec WiFi Kit 32 V3.
*/

// --- SETTINGS ---
$fn = 64;

// Card dimensions (Slabs)
// PSA: ~81.3 x 136.5 x 6.4 mm
// BGS: ~82.5 x 130.0 x 8.0 mm
card_width = 83.5;
card_depth = 9.0;    // Depth of the slot (fits both)
card_height_vis = 25; // How deep the card goes into the base

// ESP32 Module (Heltec WiFi Kit 32 V3)
esp_w = 51.0;
esp_d = 26.5;
esp_h = 11.0;
oled_w = 26.0;
oled_h = 14.0;
oled_offset_y = 6.0; // Distance from the top of the module to screen start

// Base dimensions
base_w = 95;
base_d = 60;
base_h = 35;
angle = 15; // Slant angle for the card

module case() {
    difference() {
        // Main block
        union() {
            // Main slanted body
            hull() {
                // Front thin edge
                translate([-base_w/2, 0, 0]) cube([base_w, 1, 15]);
                // Back tall edge
                translate([-base_w/2, base_d-1, 0]) cube([base_w, 1, base_h]);
                // Floor
                translate([-base_w/2, 0, 0]) cube([base_w, base_d, 2]);
            }
            
            // Front face reinforcement (where the screen is)
            hull() {
                translate([-base_w/2, 0, 0]) cube([base_w, 2, 25]);
                translate([-base_w/2, 10, 0]) cube([base_w, 1, 2]);
            }
        }

        // --- HOLLOWS ---

        // 1. Graded Card Slot
        translate([0, base_d/2 + 5, base_h - card_height_vis + 10])
        rotate([-angle, 0, 0])
        translate([-card_width/2, -card_depth/2, -10])
        cube([card_width, card_depth, 100]);

        // 2. ESP32 Internal Cavity
        // Positioned so the screen faces forward
        translate([0, 8, esp_h/2 + 5])
        cube([esp_w + 1, esp_h + 2, esp_d + 10], center=true);

        // 3. OLED Window
        // Centered on the front face
        translate([0, -1, 14])
        cube([oled_w, 10, oled_h], center=true);

        // 4. USB Cable Access (Back)
        translate([0, base_d/2, 12])
        rotate([90, 0, 0])
        hull() {
            cylinder(d=12, h=100);
            translate([0, -20, 0]) cylinder(d=12, h=100);
        }
        
        // 5. Cable path (hollowing the bottom center)
        translate([0, base_d/2, 12])
        rotate([90, 0, 0])
        cylinder(d=10, h=base_d);
    }
}

case();

// --- INSTRUCTIONS ---
/*
    Print settings:
    - Layer Height: 0.2mm
    - Infill: 15%
    - Supports: NONE (designed with angles < 45°)
    - Orientation: Print it as is (on its bottom face).
*/
