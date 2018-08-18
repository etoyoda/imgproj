#!/usr/bin/ruby

# 気象庁HP 気象衛星10分毎フルディスク赤外（大）画像の投影法パラメタ決定
# （東西・南北のスケールを最小二乗誤差になるよう決める）。
# アルゴリズムはやっつけなので応用性は保証しない。

class VProj

  include Math

  # Source: wikipedia
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

  # Source: JMA
  # central longitude
  LONZERO = 140.7

  def radian x
    PI * Float(x) / 180.0
  end

  def milli x
    (0.5 + 1000 * Float(x)).floor
  end

  def initialize argv
    @i0, @j0, @is, @js = 511.5, 512, 500, 500
    argv.each {|arg|
      case arg
      when /^i0=/ then @i0 = Float($')
      when /^j0=/ then @j0 = Float($')
      when /^is=/ then @is = Float($')
      when /^js=/ then @js = Float($')
      else $stderr.puts "unknown option #{arg}"
      end
    }
    @n = @sqsum = 0
  end

  attr_accessor :is, :js

  # from geographic lat/lon to cartesian xyz
  def geog2xyz param
    lat = radian(param[:lat])
    lon = param[:lon]
    sinlat = sin(lat)
    # capital N: normal from ellipsoid to axis
    nn = 1.0 / sqrt(1 - E2 * sinlat * sinlat)
    coslat = cos(lat)
    lam = radian(lon - LONZERO)
    x = nn * coslat * cos(lam)
    y = nn * coslat * sin(lam)
    z = B_A * B_A * nn * sinlat
    { :x => x, :y => y, :z => z }
  end

  def xyz2pix param
    x, y, z = param.values_at(:x, :y, :z)
    scale = R / (R - x * A)
    i = @i0 + y * @is * scale
    j = @j0 - z * @js * scale
    r = param.dup
    r[:scale] = scale
    r[:i] = i
    r[:j] = j
    r
  end

  def test param
    ij = xyz2pix(geog2xyz(param))
    c = sprintf("(%4i,%4i,%4i)", milli(ij[:x]), milli(ij[:y]), milli(ij[:z]))
    d = hypot(param[:i] - ij[:i], param[:j] - ij[:j])
    printf("%+05.1fN %+6.1fE %s %5.3f [%5.1f,%5.1f] %6.2fd [%3i,%3i]\n",
      param[:lat], param[:lon], c, ij[:scale], ij[:i], ij[:j], d, param[:i], param[:j])
    @n = @n.succ
    @sqsum += d * d
  end

  def meanerror
    @sqsum / @n
  end

end

def isopt ismin, ismax, js
  islo = ismin
  melo = try(js, islo)
  ishi = ismax
  mehi = try(js, ishi)
  memi = ismi = nil
  18.times {|i|
    ismi = (islo + ishi) / 2.0
    printf "=== try #%u is=%6.2f", i, ismi
    memi = try(js, ismi)
    puts [[islo, melo], [ismi, memi], [ishi, mehi]].inspect if $DEBUG
    if memi > melo and memi > mehi then
      puts "convex error"
      break
    elsif mehi > melo then
      puts "ishi := #{ismi}"
      mehi, ishi = memi, ismi
    elsif melo > mehi then
      puts "islo := #{ismi}"
      melo, islo = memi, ismi
    else
      break
    end
  }
  [ismi, memi]
end

def jsopt ismin, ismax, jsmin, jsmax
  jslo = jsmin
  islo, melo = isopt(ismin, ismax, jslo)
  jshi = jsmax
  ishi, mehi = isopt(ismin, ismax, jshi)
  memi = jsmi = nil
  18.times {|i|
    jsmi = (jslo + jshi) / 2.0
    printf "=== try #%u js=%6.2f", i, jsmi
    ismi, memi = isopt(ismin, ismax, jsmi)
    puts [[jslo, melo], [jsmi, memi], [jshi, mehi]].inspect if $DEBUG
    if memi > melo and memi > mehi then
      puts "convex error"
      break
    elsif mehi > melo then
      puts "jshi := #{jsmi}"
      mehi, jshi = memi, jsmi
    elsif melo > mehi then
      puts "jslo := #{jsmi}"
      melo, jslo = memi, jsmi
    else
      break
    end
  }
end

def try is, js
  v = VProj.new([])
  v.is = is
  v.js = js
  v.test(:lat =>  0, :lon => 140, :i => 504, :j => 512)
  v.test(:lat =>  0, :lon => 150, :i => 605, :j => 512)
  v.test(:lat => 40, :lon => 140, :i => 506, :j => 154)
  v.test(:lat =>  0, :lon => 100, :i => 149, :j => 512)
  v.test(:lat => 40, :lon => 110, :i => 298, :j => 160)
  v.test(:i => 340, :j => 231, :lat => 30, :lon => 120)
  v.test(:i => 421, :j => 229, :lat => 30, :lon => 130)
  v.test(:i => 508, :j => 50, :lat => 60, :lon => 140)
  v.test(:i => 507, :j => 929, :lat => -50, :lon => 140)
  v.test(:i =>  48, :j => 512, :lat => 0, :lon => 80)
  v.test(:i => 970, :j => 512, :lat => 0, :lon => 200)
  v.test(:i => 774, :j => 164, :lat => 40, :lon => 180)
  v.test(:i => 506, :j => 229, :lat => 30, :lon => 140)
  v.test(:i => 298, :j => 863, :lat => -40, :lon => 110)
  v.test(:i => 774, :j => 859, :lat => -40, :lon => 180)
  m = v.meanerror
  printf("RMSE (is:%6.2f js:%6.2f) = %6.2f\n", is, js, Math::sqrt(m))
  m
end

jsopt 256, 512, 256, 512
