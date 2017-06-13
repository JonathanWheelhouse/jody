exe = 'jody'
sources = [
    'brands.c',
    'brands.h',
    'cursor.c',
    'cursor.h',
    'gamedefs.h',
    'main.c',
    'sprite.h',
    'sprite.c',
    'surface.c',
    'surface.h',
    'util.c',
    'util.h',
    'wheelhouse.c',
    'wheelhouse.h'
]
executable(
    exe,
    sources,
    install: true,
    dependencies: [sdl, sdlimage]
)

imgdir = join_paths(get_option('datadir'), exe, 'img') # share/jody/img
message(imgdir)

install_data(
  'img/icon.png',
  'img/wheelhouse_image_black_with_white_lines.png',
  'img/wheelhouse_image_inverted.png',
  'img/wheelhouse_image_transparent.png',
  install_dir : imgdir
)

install_subdir('img/background', install_dir : imgdir)
install_subdir('img/cloud', install_dir : imgdir)
install_subdir('img/cow_black', install_dir : imgdir)
install_subdir('img/cow_brown', install_dir : imgdir)
install_subdir('img/cow_orange', install_dir : imgdir)
install_subdir('img/cow_red', install_dir : imgdir)
install_subdir('img/cow_white', install_dir : imgdir)
install_subdir('img/cow_yellow', install_dir : imgdir)
install_subdir('img/cow_yellow_with_red_outline', install_dir : imgdir)
install_subdir('img/kangaroo_black', install_dir : imgdir)
install_subdir('img/kangaroo_grey', install_dir : imgdir)
install_subdir('img/kangaroo_red', install_dir : imgdir)
