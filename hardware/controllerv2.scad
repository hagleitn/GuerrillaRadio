use <lid.scad>;
use <fillet.scad>;

debug = false;
no_fillet = false;

print = true;

$fa = 12; // 12 for printing
$fs = 2; // 2 for printing
$fn = 0;

render_outer = false;
render_full = false;
render_all = true;

// thickness of the shell
wall = 3;

// handle dimensions
handle_max_width = 35;
handle_min_width = 20;
handle_length = 75;

// used to avoid phantom wall when cutting objects
space = 1;

// dimensions of the 4 switches
switch_r = 4;
switch_offset = [7,7]; // relative to top and bottom corner

// dimensions of the led
led_r = 3;
led_offset = 2; // relative to bottom of the gimbal

// dimensions of the module bay
module_bay = [60,45,18];
cutout = [13,4];
cutout_offset = [3,3];
module_bay_lip = 3;

// gimbal dimensions
gimbal = [58,58,30];
gimbal_r = 25;

// dimensions of 4 gimbal screws per gimbal
gimbal_screw_offset = [5,5];
gimbal_screw_r = 1.5;
gimbal_space = [0,0];

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
        gimbal[2]];
        
// needs to be bigger than model
box_size = (max(dims[0],dims[1],dims[2]) + handle_length) * 10;
        
module module_bay_cutout() {
    cube([15,2*wall,16], center=true);
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

//!module_bay();

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
        scale([0.6,0.6,1]) sphere(d[2]/2 - offset);
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

module handle(debug=false) {
    addition = (handle_length+dims[1])*sin(side_angle);
    tilt_vector = tilt_angle*[cos(side_angle),sin(side_angle),0];
    sf = 0.75;
    scale_vector = [1,1,sf];
    
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

module core_with_bay_and_handles(debug=false) {
    union() {
        core_with_bay(debug=debug);
        handle(debug=debug);
        mirror() handle(debug=debug);
    }
}

module skin(debug=false, thickness=5, box=box_size) {
    if (!debug) intersection() { 
        children(); 
        minkowski() {
            cube(2*thickness,center=true);
            //cylinder(thickness,thickness,0,$fn=3,center=true);
            difference() { 
                cube(box,center=true); 
                children(); 
            } 
        } 
    }
    
    if (debug) children();
}

module cutouts() {
    
    h = 2*wall+2*space;

    // gimbal cutout
    translate([gimbal[0]/2,0,dims[2]/2]) 
        cylinder(h, r=gimbal_r, center=true);
    
    // gimbal screw cutout
    translate([gimbal_screw_offset[0],gimbal[1]/2-gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);
    
    translate([gimbal[0] - gimbal_screw_offset[0],gimbal[1]/2-gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);

    translate([gimbal_screw_offset[0],-gimbal[1]/2+gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);
    
    translate([gimbal[0] - gimbal_screw_offset[0],-gimbal[1]/2+gimbal_screw_offset[1],dims[2]/2]) 
        cylinder(h, r=gimbal_screw_r, center=true);

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
            cube([switch_cutout[0], switch_cutout[1], gimbal[1]/2], center=true);
            
     // led
     translate([0,-gimbal[1]/2-led_offset-led_r,gimbal[2]/2]) 
        cylinder(h, r=led_r, center=true);
        
     // trim switches
     translate([-dims[0]/2+handle_min_width+trim_switch_r-5, -dims[1]/2-16, 0]) 
        rotate([0,45,-25]) 
            translate([0,0,dims[2]/2]) 
                cylinder(h=dims[2], r=trim_switch_r, center=true);
}

module screw_cylinder(h=30, solid=false) {
    spacing = 2;
    l = (h-separator)/2;
    union() {
        translate([0,0,separator/2+l/2+spacing/2]) difference() {
            cylinder(l, r=holder_r, center=true);
            if (!solid) cylinder(l, r=nut_r, $fn=6, center=true);
        }
        translate([0,0,separator/2+spacing/2]) difference() {
            cylinder(separator/2, r=holder_r, center=true);
            if (!solid) cylinder(separator/2, r=screw_body_r, center=true);
        }
        translate([0,0,-separator/2-spacing/2]) difference() {
            cylinder(separator/2, r=holder_r, center=true);
            if (!solid) cylinder(separator/2, r=screw_body_r, center=true);
        }
        translate([0,0,-separator/2-l/2-spacing/2]) difference() {
            cylinder(l, r=holder_r, center=true);
            if (!solid) cylinder(l, r=screw_head_r, center=true);
        }
    }
}

module screw__(solid=false) {
    addition = (handle_length+dims[1])*sin(side_angle);
    sf = 0.75;

    translate([dims[0]/2+addition-handle_min_width/2,
               -dims[1]/2-handle_length-handle_min_width+4,
               dims[2]/2-sf*handle_min_width/2-3])
        rotate([tilt_angle,0,0]) 
            translate([0,0,-handle_max_width-7])
                mirror([0,0,1]) 
                    screw_cylinder(2*handle_max_width, solid=solid);
}

module screw___(solid=false) {   
    screw_offset = -3;
    
    translate([-module_bay[0]/2+screw_offset-holder_r,gimbal[1]/2+holder_r+1,0])
        mirror([0,0,1]) 
            screw_cylinder(2*(gimbal[2]+module_bay[2]+wall), solid=solid);
}

module screw_(solid=false, debug=false) {
    intersection() {
        union() {
            screw__(solid=solid);
            screw___(solid=solid);
        }
        core_with_bay_and_handles(debug=debug);
    }
}

//screw_cylinder();
module screws(solid=false, debug=false) {
    screw_(solid=solid, debug=debug);
    mirror() screw_(solid=solid, debug=debug);
}
                
module body(solid = false, w = wall) {
    difference() {
        union() {
            difference() {
                skin(debug=debug,thickness=w, box=box_size) {
                    core_with_bay_and_handles(debug=debug);
                }
                translate([0,0,-dims[2]/2]) rotate([0,180,90])
                    module_bay(true);
                screws(solid=true, debug=debug);
            }
            translate([0,0,-dims[2]/2]) rotate([0,180,90])
                    module_bay(solid);
            screws(solid=false, debug=debug);
        }
        
        if (!solid) cutouts();
        if (!solid) mirror() cutouts();
    }
}

module full_skin() render() {
    if (render_full || render_all) {
        body(w=wall);
    } else {
        import("full_skin.stl");
    }
}

module inner_skin() render() {
    difference() {
        full_skin();
        outer_skin();
    }
}

module outer_skin() render() {
    if (render_outer || render_all) {
        body(w=wall/2);
    } else {
        import("outer_skin.stl");
    }
}

module bottom(box=box_size) {
    union () {
        difference() {
            outer_skin();
            translate([0,0,box/2]) cube(box, center=true);
            translate([0,-gimbal[1]/2,0])
                rotate([tilt_angle,0,0]) 
                    translate([0,0,box/2]) 
                        cube(box, center=true);
        }

        difference() {
            full_skin();
            translate([0,0,box/2-wall/2]) cube(box, center=true);
            translate([0,-gimbal[1]/2,0])
                rotate([tilt_angle,0,0]) 
                    translate([0,0,box/2-wall/2]) 
                        cube(box, center=true);
        }
    }
}

module cut_in_halves() {
    union() {
        translate([-100,0,gimbal[2]/2]) difference() { 
            bottom(box=box_size); 
            battery_compartments(solid=true); 
        }
    
        translate([100,0,0]) difference() { 
            full_skin(); 
            bottom(box=box_size); 
        }
    }
}


module battery_compartments(solid=false) {
    union() {
        battery_compartment(solid);
        mirror() battery_compartment(solid);
    }
}

batt = [61,21,24];
batt_pos = [-dims[0]/2-2,-66,-16];
batt_rot = [90,26,-101];

module battery_compartment_(solid = false) {
    offset = solid ? wall : 0;
    union() {
        intersection() {
            outer_skin();
            translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                rotate(batt_rot)
                translate([0,0,offset/2])
                cube([batt[0]+wall,batt[1],batt[2]-offset],center=true);
        }
        
        intersection() {
            inner_skin();
            translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                rotate(batt_rot) 
                translate([0,0,wall/4+offset/2])
                cube([batt[0],batt[1],batt[2]+wall/2-offset],center=true);
        }            
        
        // cutout to open compartment
        if (solid) {
            translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                rotate(batt_rot)
                    union() {
                        translate([0,0,wall/2])
                            cube([batt[0],batt[1],batt[2]-wall],center=true);
                        translate([0,0,-batt[2]/2]) 
                            cube([batt[2]/2,batt[1],2*wall],center=true);
                    }
        }
    }
}

module battery_compartment(solid=false) {
    difference() {
        union() {
            difference() {
                battery_compartment_(solid);
                
                // cutout block to be able to unlatch
                if (!solid) {
                    translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                        rotate(batt_rot)
                        translate([0,0,-batt[2]/2+wall/2+wall])
                            cube([batt[2],batt[1],wall],center=true);
                }
            }
        
            if (!solid) {
                scale([1.01,1.01,1.01])
                union() {
                    hull() {
                        intersection() {
                            battery_compartment_();
                            translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                                rotate(batt_rot)
                                translate([0,0,-batt[2]/2+wall/4])
                                    cube([batt[2],batt[1],wall/2],center=true);
                        }
                        
                        translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                            rotate(batt_rot)
                            translate([0,batt[1]/2-batt[1]/96,-batt[2]/2+wall/4+2*wall])
                                cube([batt[2],batt[1]/48,wall/2],center=true);
                    }
                    
                    hull() {
                        intersection() {
                            battery_compartment_();
                            translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                                rotate(batt_rot)
                                translate([0,0,-batt[2]/2+wall/4+2*wall])
                                    cube([batt[2],batt[1],wall/2],center=true);
                        }
                        
                        translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                            rotate(batt_rot)
                            translate([0,batt[1]/2-batt[1]/96,-batt[2]/2+wall/4+2*wall])
                                cube([batt[2],batt[1]/48,wall/2],center=true);
            
                    }
                }
            }
        }
        
        if (!solid) {
            difference() {
                intersection() {
                    battery_compartment_();
                    translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                        rotate(batt_rot)
                        translate([0,0,-batt[2]/2+wall/2])
                            cube([batt[2],batt[1],wall+2],center=true);
                }

                intersection() {
                    battery_compartment_();
                    translate([batt_pos[0],batt_pos[1],batt_pos[2]-batt[2]/2]) 
                        rotate(batt_rot)
                        translate([0,0,-batt[2]/2+wall/2])
                            cube([batt[2]/2,batt[1],wall+2],center=true);
                }
            }
        }
    }
}

if (render_outer) {
    outer_skin();
} else if (render_full) {
    full_skin();
} else {
    union() {
        cut_in_halves();
        translate([-100,dims[1]/2+2*module_bay[1],0]) rotate([0,180,90]) module_cover();
        translate([-100,-dims[1]/2-2*module_bay[1],0]) battery_compartments();
    }
}