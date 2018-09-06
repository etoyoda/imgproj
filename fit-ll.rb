$db = []
$isize = $jsize = 1024

ARGF.each_line {|line|
  case line
  when /^#isize,(\d+)/ then
    $isize = Integer($1)
  when /^#jsize,(\d+)/ then
    $jsize = Integer($1)
  when /^\s*[-0-9]/ then
    tokens = line.chomp.split(/,/, 5)
    i, j, lat, lon = tokens[0,4].map{|s| Float(s) }
    $db.push [i, j, lat, lon]
  end
}

def mse r, ii, ix, iavr, xavr
  sse = 0.0
  $db.each{|row|
    igues = iavr + r * (row[ix] - xavr)
    e = row[ii] - igues
    sse += e * e
  }
  sse / $db.size
end

def refine_core(r0, r9, e0, e9, ii, ix, iavr, xavr, depth)
  r5 = 0.5 * (r0 + r9)
  e5 = mse(r5, ii, ix, iavr, xavr)
  if e5 > e0 and e5 > e9
    $stderr.puts "saturated"
    return r5
  elsif e0 == e9
    $stderr.puts "e0 == e9"
    return r5
  elsif depth > 16
    return r5
  elsif e0 < e9
    refine_core(r0, r5, e0, e5, ii, ix, iavr, xavr, depth+1)
  else
    refine_core(r5, r9, e5, e9, ii, ix, iavr, xavr, depth+1)
  end
end

def refine r, ii, ix, iavr, xavr
  r0 = r * 0.75
  r9 = r * 1.25
  e0 = mse(r0, ii, ix, iavr, xavr)
  e9 = mse(r9, ii, ix, iavr, xavr)
  refine_core(r0, r9, e0, e9, ii, ix, iavr, xavr, 0)
end

def fit ii, ix, si, sx, isize
  isum = 0.0
  xsum = 0.0
  $db.each{|row| isum += row[ii];  xsum += row[ix] }
  iavr = isum / $db.size
  xavr = xsum / $db.size
  tsum = 0.0
  tnum = 0
  $db.each{|row|
    next if row[ix] == xavr
    tsum += (row[ii] - iavr) / (row[ix] - xavr)
    tnum += 1
  }
  r = tsum / tnum
  printf "guess r=%9.5g\n", r
  r = refine(r, ii, ix, iavr, xavr)
  printf "%s = %9.5g + %9.5g * (%s - %9.5g)\n", si, iavr, r, sx, xavr
  izero = iavr - r * xavr
  printf "%s = %9.3f + %9.5f %s\n", si, izero, r, sx
  imax = isize - 1
  xmin = xavr - iavr / r
  xmax = xavr + (imax - iavr) / r
  printf "%s[0..%u] <- %s %9.4f..%9.4f\n", si, imax, sx, xmin, xmax
end

fit 0, 3, "i", "lon", $isize
fit 1, 2, "j", "lat", $jsize
