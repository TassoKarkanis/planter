-- create the particle list
local plist = create_particle_list()

-- create a sphere
local position = {0, 0, 0}
local radius = 10
add_particle(plist, position, radius)
   
-- render the spheres into an SDF
local grid = create_grid(2.0)
particles_to_sdf(plist, grid)

-- generate the output file
write_obj("output.obj", grid)
