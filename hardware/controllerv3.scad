use <lid.scad>;
use <fillet.scad>;

debug = false;
no_fillet = false;

print = true;

$fa = 12;//3; // 12 for printing
$fs = 2;//0.5; // 2 for printing
$fn = 0;

// thickness of the shell
wall = 1.5;

// handle dimensions
handle_max_width = 35;
handle_min_width = 20;
handle_length = 75;

// used to avoid phantom wall when cutting objects
space = 1;

// dimensions of the 4 switches
switch_r = 3;
switch_offset = [10,10]; // relative to top and bottom corner

// dimensions of the module bay
module_bay = [60,45,18];
cutout = [13,4];
cutout_offset = [3,3];
module_bay_lip = 3;

// gimbal dimensions
gimbal = [60,60,30];
gimbal_r = 24;

// dimensions of the led
led_r = 3;
led_offset = -gimbal[1]/2; // relative to bottom of the gimbal

electronics_bay = 5;    

// dimensions of 4 gimbal screws per gimbal
gimbal_screw_offset = [3,3];
gimbal_screw_r = 3;

gimbal_space = [0,0];

gimbal_screw_offset_large = [7,7];
gimbal_screw_r_large = 1.5;

// power switch dimension
switch_cutout = [10,20];

// angle of left and right side of controller
side_angle = 10;

// inclination of the handles
tilt_angle = 25;

// screw dimensions
nut_r = 2.1;
screw_head_r = 2;
screw_body_r = 1;
holder_r = 3.5;
separator = 4;

// trim switches
trim_switch_r = 6.5/2;

dims = [gimbal[0]*2+gimbal_space[0]*2,
        max(gimbal[1]+2*gimbal_space[1], module_bay[1]+2*wall+2*module_bay_lip),
        gimbal[2]+2*wall+electronics_bay];
        
// needs to be bigger than model
box_size = (max(dims[0],dims[1],dims[2]) + handle_length) * 10;
        
module module_bay_cutout() {
    cube([15,3*wall,16], center=true);
}

module module_bay(solid=false) {
    translate([0,0,-wall])
    difference() {
        union() {
            translate([0,0,module_bay[2]/2+wall/2])
                cube([module_bay[0]+2*wall, module_bay[1]+2*wall, module_bay[2]+wall], center=true);
            difference() {
                translate([0,0,(module_bay_lip+wall)/2+module_bay[2]])
                    cube([module_bay[0]+2*wall+2*module_bay_lip, 
                        module_bay[1]+2*wall+2*module_bay_lip, 
                        module_bay_lip+wall], center=true);
                if (!solid) 
                    translate([0,0,module_bay[2]/2+wall])
                        cube([module_bay[0]+2*wall, 
                              module_bay[1]+2*wall, 
                              module_bay[2]+wall], center=true);
            }
            
            if (solid) {
                translate([0,module_bay[1]/2+wall,wall+8]) module_bay_cutout();
                translate([0,-module_bay[1]/2-wall,wall+8]) module_bay_cutout();
            }            
        }
        
        if (!solid) {
            translate([0,0,module_bay[2]+module_bay_lip/2+wall])
                cube([module_bay[0]+2*module_bay_lip, 
                      module_bay[1]+2*module_bay_lip, 
                      module_bay_lip], center=true);
        
            translate([0,0,wall+module_bay[2]/2]) 
                cube([module_bay[0], module_bay[1], module_bay[2]], center=true);
        
        
            translate([module_bay[0]/2-cutout[0]-cutout_offset[0], 
                       module_bay[1]/2-cutout[1]-cutout_offset[1],
                       -space])
                cube([cutout[0], cutout[1], wall+2*space]);
            
            translate([0,module_bay[1]/2+wall,wall+8]) module_bay_cutout();
            translate([0,-module_bay[1]/2-wall,wall+8]) module_bay_cutout();
        } 
    }
}

//module_bay(true);

module core(d = [10,10,10], offset = 0, solid=false) {
    scale_cylinder = [1,0.1,1];
    
    addition = sin(side_angle)*d[1];
    
    minkowski() {
        hull() {
            translate([0,-d[1]/2,0]) scale(scale_cylinder) 
                cylinder(1,d[0]/2+addition,d[0]/2,center=true);
            translate([0,d[1]/2,0]) scale(scale_cylinder) 
                cylinder(1,r=d[0]/2,center=true);
        }
        scale([0.6,0.6,1]) 
            sphere(d[2]/2 - offset);
    }
}

module core_with_bay(solid=false, debug=false) {
    if (!debug && !no_fillet) {
        fillet(r=25,steps=20) {
            core(dims, solid=true);
            translate([0,0,-dims[2]/2]) rotate([0,180,90])
                module_bay(solid=true);
        }
    } else {
        union() {
            core(dims, solid=true);
            translate([0,0,-dims[2]/2]) rotate([0,180,90])
                module_bay(solid=true);
        }
    }
}

//core_with_bay();

batt = [75,21,24];
batt_pos = [-dims[0]/2+25,-24,-9];
batt_rot = [90,20,-108];

module handle(debug=false) {
    addition = (handle_length+dims[1])*sin(side_angle);
    tilt_vector = tilt_angle*[cos(side_angle),sin(side_angle),0];
    sf = 0.75;
    scale_vector = [1,1,sf];
    
    difference() {
        hull() {
            difference() {
                core_with_bay(debug=debug);
                translate([-handle_max_width,0,0]) 
                    rotate([0,0,side_angle+5]) 
                        cube(dims[0],center=true);
                translate([0,11,0]) 
                    rotate([0,0,side_angle+5]) 
                        cube([2*dims[0],dims[0],dims[0]],center=true);
            }
            scale(scale_vector)
            rotate(tilt_vector)
            translate([dims[0]/2+addition-handle_min_width/2,
                       -dims[1]/2-handle_length,
                       dims[2]/2-sf*handle_min_width/2])
                sphere(handle_min_width);
        }
    }
}

//handle();

module core_with_bay_and_handles(debug=false) {
    union() {
        core_with_bay(debug=debug);
        handle(debug=debug);
        mirror() handle(debug=debug);
    }
}

//core_with_bay_and_handles();

module trapezoid(ds, ext, center=true) {
    addition = sin(side_angle)*(ds[1]+ext);
    hull() {
        translate([-ds[0]/2,ds[1]/2+ext,0]) cylinder(ds[2], r=wall, center=center);
        translate([-ds[0]/2,-ds[1]/2+wall,0]) cylinder(ds[2], r=wall, center=center);
        translate([ds[0]/2,ds[1]/2+ext,0]) cylinder(ds[2], r=wall, center=center);
        translate([ds[0]/2+addition,-ds[1]/2+wall,0]) cylinder(ds[2], r=wall, center=center);
    }
}

module cutouts() {
    
    h = 2*wall+2*space;

    // gimbal cutout
    translate([gimbal[0]/2,0,dims[2]/2]) 
        cylinder(h, r=gimbal_r, center=true);
    
    translate([gimbal[0]/2,0,-wall/2]) 
        trapezoid([gimbal[0],gimbal[1],gimbal[2]+electronics_bay], 0, center=true);

    translate([gimbal[0]/2,0,-wall/2]) 
        trapezoid([gimbal[0],gimbal[1],(gimbal[2]+electronics_bay)*4/5], 6, center=true);
    
    // gimbal screw cutout
    translate([gimbal_screw_offset[0],gimbal[1]/2-gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);
    
    translate([gimbal[0] - gimbal_screw_offset[0],gimbal[1]/2-gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);

    translate([gimbal_screw_offset[0],-gimbal[1]/2+gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);
    
    translate([gimbal[0] - gimbal_screw_offset[0],-gimbal[1]/2+gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);
        
    // gimbal screw cutout
    translate([gimbal_screw_offset_large[0],gimbal[1]/2-gimbal_screw_offset_large[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r_large, center=true);
    
    translate([gimbal[0] - gimbal_screw_offset_large[0],gimbal[1]/2-gimbal_screw_offset_large[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r_large, center=true);

    translate([gimbal_screw_offset_large[0],-gimbal[1]/2+gimbal_screw_offset_large[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r_large, center=true);
    
    translate([gimbal[0] - gimbal_screw_offset_large[0],-gimbal[1]/2+gimbal_screw_offset_large[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r_large, center=true);


    // switches
    translate([-gimbal[0]+switch_offset[0], gimbal[1]/2, dims[2]/2-switch_offset[1]]) 
        rotate ([90,0,0]) 
            cylinder(gimbal[1]/2, r=switch_r, center=true);

    translate([-gimbal[0]+switch_offset[0], gimbal[1]/2, -dims[2]/2+switch_offset[1]]) 
        rotate ([90,0,0]) 
            cylinder(gimbal[1]/2, r=switch_r, center=true);
            
     // power switch
     translate([0,dims[1]/2+dims[2]/2,0]) 
        rotate([0,90,90])
            cube([switch_cutout[0], switch_cutout[1], gimbal[1]], center=true);
            
     // led
     translate([0,-gimbal[1]/2-led_offset-led_r,gimbal[2]/2+electronics_bay/2]) 
        cylinder(h, r=led_r, center=true);
}
                
module body(solid = false, w = wall) {
    difference() {
        union() {
            difference() {
                core_with_bay_and_handles(debug=debug);
                translate([0,0,-dims[2]/2]) rotate([0,180,90])
                    module_bay(true);
            }
            translate([0,0,-dims[2]/2]) rotate([0,180,90])
                    module_bay(solid);
        }
        
        if (!solid) cutouts();
        if (!solid) mirror() cutouts();
        
        translate(batt_pos) rotate(batt_rot) cube(batt);
        mirror() translate(batt_pos) rotate(batt_rot) cube(batt);
    }
}

//body();

module bottom(box=box_size) {
    union () {
        difference() {
            body();
            translate([0,0,box/2]) cube(box, center=true);
            translate([0,-gimbal[1]/2,0])
                rotate([tilt_angle,0,0]) 
                    translate([0,0,box/2]) 
                        cube(box, center=true);
        }

        difference() {
            body();
            translate([0,0,box/2-wall/2]) cube(box, center=true);
            translate([0,-gimbal[1]/2,0])
                rotate([tilt_angle,0,0]) 
                    translate([0,0,box/2-wall/2]) 
                        cube(box, center=true);
        }
    }
}

module cut_in_quarters() {
    union() {
        translate([-110,0,gimbal[2]/2]) difference() { 
            difference() {
                bottom(box=box_size); 
                translate([250,0,0]) cube([500,500,500],center=true);
            }
        }
        
        translate([-100,0,gimbal[2]/2]) difference() { 
            difference() {
                bottom(box=box_size); 
                translate([-250,0,0]) cube([500,500,500],center=true);
            }
        }

        translate([110,0,0]) difference() { 
            body(); 
            bottom(box=box_size); 
            translate([-250,0,0]) cube([500,500,500],center=true);
        }
        
        translate([100,0,0]) difference() { 
            body(); 
            bottom(box=box_size); 
            translate([250,0,0]) cube([500,500,500],center=true);
        }

    }
}

union() {
    cut_in_quarters();
    translate([-100,dims[1]/2+2*module_bay[1],0]) rotate([0,180,90]) module_cover(wall);
}