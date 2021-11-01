require("math")

--
-- 3D Vectors
--
-- Vectors are 3D Cartesian coordinates represented as simple array
-- tables with three numbers.  Other code tends to hold on to vectors,
-- so we don't mutate them.  Instead we define operations that create
-- new vectors.

-- returns -p
function vec_neg(p)
   return {-p[1], -p[2], -p[3]}
end

-- returns p + q
function vec_add(p, q)
   return {p[1] + q[1],
	   p[2] + q[2],
	   p[3] + q[3]}
end

-- return a*p
function vec_mult(a, p)
   return {a*p[1], a*p[2], a*p[3]}
end

-- returns a*p + b*q
function vec_madd(a, p, b, q)
   return {a*p[1] + b*q[1],
	   a*p[2] + b*q[2],
	   a*p[3] + b*q[3]}
end


--
-- Scalar-Valued Functions
--
-- Scalar-valued functions are evaluated on the interval [0,1] and
-- return a single number.  The functions below return scalar-valued
-- functions (kind of meta, right?).

-- returns a constant function
function constant(a)
   local f = function(t)
      return a
   end
   return f
end

-- returns function that linearly ramps from a to b
function ramp(a, b)
   local f = function(t1)
      local t2 = 1.0 - t1
      return a*t1 + b*t2
   end
   return f
end

-- returns a sine wave
function sine(start, stop, scale)
   local f = function(t1)
      local t2 = 1.0 - t1
      local theta = t1*start + t2*stop
      return scale * math.sin(theta)
   end
   return f
end

-- returns a function that is the linear combination of two scalar functions
function scalar_combine(r1, func1, r2, func2)
   local f = function(t)
      val1 = func1(t)
      val2 = func2(t)
      return r1*val1 + r2*val2
   end
   return f
end

--
-- Vector-Valued Functions
--
-- Vector-valued functions are evaluated on the interval [0,1] and
-- return a 3D vector.  Again, the functions below all return
-- vector-valued functions with certain properties.

-- returns a line from p1 to p2
function line(p1, p2)
   local f = function(t1)
      local t2 = 1.0 - t1
      return vec_madd(t1, p1, t2, p2)
   end
   return f
end

-- returns a helix
function helix(center, radius_func, pitch_func, revolutions, offset)
   local f = function(t)
      local theta = offset + 2 * math.pi * t * revolutions
      local radius = radius_func(t)
      local pitch = pitch_func(t)
      local v = {radius * math.cos(theta),
		 radius * math.sin(theta),
		 pitch}
      return vec_add(center, v)
   end
   return f
end


--
-- Shapes
--
-- Shapes are functions evaluated on the interval [0,1] that return
-- two items: a position and a radius.  The pair of return values can
-- be thought of as defining a sphere at a point in space with a
-- particular radius.  These functions are intended to define a 3D
-- geometric shape as the union of the spheres that they can return.

function shape(position_func, radius_func)
   local f = function(t)
      return position_func(t), radius_func(t)
   end
   return f
end

-- returns a rounded cone or capsule given end-point vectors and radii values
function cone(p1, p2, r1, r2)
   local curve = line(p1, p2)
   local radius = ramp(r1, r2)
   local f = function(t)
      return curve(t), radius(t)
   end
   return f
end

-- returns a helix with constant major radius, and minor radius ramp, pitch ramp
function helix1(center, major_r, minor_r1, minor_r2, pitch1, pitch2, revs, offset)
   local major_radius = constant(major_r)
   local pitch = ramp(pitch1, pitch2)
   local curve = helix(center, major_radius, pitch, revs, offset)
   
   local radius = ramp(minor_r1, minor_r2)
   return shape(curve, radius)
end

--
-- Visualizing Shapes
--
-- In order to visualize shapes in 3D, the shape functions are
-- repeatedly sampled and the resulting spheres are stored in a
-- particle list.  The particle list can then be rendered into an
-- OpenVDB signed distance function.  An isosurface of the SDF can
-- then be converted to an OBJ file containing triangles, which can be
-- drawn using various software.
--
-- Some of the required operations are implemented in C/C++ and are
-- available to be called from Lua with "bindings".

-- (C function) create a particle list and return it
-- function create_particle_list()

-- (C function) create an OpenVDB grid with default value representing the SDF
-- function create_grid(background_value)

-- (C function) add a particle (sphere) to a particle list
-- function add_particle(plist, position, radius)

-- (C function) render a particle list into an SDF
-- function particles_to_sdf(plist, grid)

-- (C function) write the triangles of an SDF isosurface into an OBJ file
-- function write_obj(filename, grid)

-- evaluate a shape function and store the particles into the particle list
function sample_shape(plist, shape, num_samples)
   for i = 0, num_samples do
      local t = i/num_samples
      p, r = shape(t)
      add_particle(plist, p, r)
   end
end

-- evaluate an array of shape functions and store the particles into the list
function sample_shapes(plist, shapes, num_samples)
   for i = 1, #shapes do
      sample_shape(plist, shapes[i], num_samples)
   end
end
