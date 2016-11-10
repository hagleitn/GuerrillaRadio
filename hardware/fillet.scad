// Minkowski-based offset operator
module offset_3d_shape(r) {
    intersection() {
        cube([2*r,2*r,2*r],center=true);
        rotate([45,0,0]) intersection() {
            cube([4*r,2*r,4*r],center=true);
            cube([4*r,4*r,2*r],center=true);
        }
        rotate([0,45,0]) intersection() {
            cube([4*r,4*r,2*r],center=true);
            cube([2*r,4*r,4*r],center=true);
        }
        rotate([0,0,45]) intersection() {
            cube([2*r,4*r,4*r],center=true);
            cube([4*r,2*r,4*r],center=true);
        }
    }
}

module offset_3d(r=0) {
    if(r>0) {
        for(i=[0:$children-1]) render() minkowski() {
            children(i);
            offset_3d_shape(r=r);
        }
    } else if(r<0) {
        for(i=[0:$children-1]) difference() {
                children(i);
                offset_3d(r=-r) render() difference() {
                    offset_3d(r=-r) children(i);
                    children(i);
                }
        }
    } else {
        children();
    }
}

module fillet(r=1.0,steps=3,include=true) {
  if(include) for (k=[0:$children-1]) {
	children(k);
  }
  for (i=[0:$children-2] ) {
    for(j=[i+1:$children-1] ) {
	fillet_two(r=r,steps=steps) {
	  children(i);
	  children(j);
	  intersection() {
		children(i);
		children(j);
	  }
	}
    }
  }
}

module fillet_two(r=1.0,steps=3) {
  for(step=[1:steps]) {
	hull() {
	  render() intersection() {
		children(0);
		offset_3d(r=r*step/steps) children(2);
	  }
	  render() intersection() {
		children(1);
		offset_3d(r=r*(steps-step+1)/steps) children(2);
	  }
	}
  }
}
