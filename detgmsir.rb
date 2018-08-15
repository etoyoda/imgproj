#!/usr/bin/ruby

include Math

# flattening
Finv = 298.257_222_100_882_711_243
# eccentricity squared
E2 = 0.00669_43800_22903_41574_95749_48586_28930_62124_43890
# aspect ratio
B_A = 0.996_647_189_318_816_362
# semi-major axis
A = 6_371_137.0
# radius of geostationary orbit
R = 42_164_000.0

# central longitude
LONZERO = 140.7

def radian x
  PI * x / 180.0
end

# from geographic lat/lon to cartesian xyz
def geog2xyz obj
  lat = obj[:lat]
  lon = obj[:lon]
  sinlat = sin(radian(lat))
  # capital N: normal from ellipsoid to axis
  nn = 1.0 / sqrt(1 - E2 * sinlat * sinlat)
  coslat = cos(radian(lat))
  lam = radian(lon - LONZERO)
  x = nn * coslat * cos(lam)
  y = nn * coslat * sin(lam)
  z = B_A * B_A * nn * sinlat
  { :x => x, :y => y, :z => z }
end

def xyz2pix obj
  obj
end

def geog2pix obj
  xyz2pix(geog2xyz(obj)
end

p geog2pix(:lat => 110, :lon => 40)
