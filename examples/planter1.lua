function create_planter(plist)
   local shapes = {}
   
   local center = {0, 0, 0}
   local helix_radius
   do
      local r = 100
      local x = constant(1)
      local y = sine(0, math.pi, 1)
      helix_radius = scalar_combine(r, x, -0.8*r, y)
   end
		      
   local pitch = ramp(0, 600)
   local rev = 2 - 1/12.0
   local curve = helix(center, helix_radius, pitch, rev, 0)
   local radius = ramp(10, 2.5)
   table.insert(shapes, shape(curve, radius))

   curve = helix(center, helix_radius, pitch, rev, 2*math.pi/3)
   table.insert(shapes, shape(curve, radius))

   curve = helix(center, helix_radius, pitch, rev, 4*math.pi/3);
   table.insert(shapes, shape(curve, radius))

   -- and three more the other direction
   curve = helix(center, helix_radius, pitch, -rev, 1*math.pi/3);
   table.insert(shapes, shape(curve, radius))
   
   curve = helix(center, helix_radius, pitch, -rev, 3*math.pi/3);
   table.insert(shapes, shape(curve, radius))

   curve = helix(center, helix_radius, pitch, -rev, 5*math.pi/3);
   table.insert(shapes, shape(curve, radius))
   
   -- sample the shapes
   local num_samples = 500
   sample_shapes(plist, shapes, num_samples)
end

-- create the particle list
local plist = create_particle_list()

-- create a helix
create_planter(plist)
   
-- render the spheres into an SDF
local grid = create_grid(2.0)
particles_to_sdf(plist, grid)

-- generate the output file
write_obj("output.obj", grid)
