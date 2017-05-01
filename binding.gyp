{
  "targets": [
    {
      "target_name": "gui",
      "sources": [
        # "src/cpp/addon.cc",
        "src/cpp/binding.cc",
        "src/cpp/EventDispatcher.cc",
        "src/cpp/event.cc",
        "src/cpp/gui.cc"
      ],
       "conditions":[
        ['OS=="linux"',{
            'cflags':[
              '<!@(pkg-config --cflags keybinder-3.0 gtk+-3.0 gtkmm-3.0)'
            ],
            'cflags_cc': ['-fexceptions'],
            'cflags_cc!': [ '-fno-rtti' ],
            'ldflags':[
              '<!@(pkg-config --libs gtkmm-3.0 keybinder-3.0)'
            ]
          }
        ]
      ],
      "libraries":[
        '<!@(pkg-config --libs gtkmm-3.0)'
      ],
      "include_dirs": ["<!(node -e \"require('nan')\")"]
    }
  ]
}
