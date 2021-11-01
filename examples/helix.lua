function create_helix(plist)
   -- bottom center of the helix
   local center = {0, 0, 0}

   -- major radius (losely speaking, the radius of cross section of the coils)
   local major_radius = 100

   -- starting and ending minor radius (thickness of volume band)
   local minor_radius1 = 10
   local minor_radius2 = 25

   -- starting and ending pitch
   local pitch1 = 200
   local pitch2 = 400

   -- number of revolutions and starting offset
   local revs = 5
   local offset = 0

   -- make the shape
   local shape = helix1(center, major_radius, minor_radius1, minor_radius2, pitch1, pitch2, revs, offset)

   -- sample it
   local num_samples = 300
   sample_shape(plist, shape, num_samples)
end

-- create the particle list
local plist = create_particle_list()

-- create a helix
create_helix(plist)
   
-- render the spheres into an SDF
local grid = create_grid(2.0)
particles_to_sdf(plist, grid)

-- generate the output file
write_obj("output.obj", grid)
