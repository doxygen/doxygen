sub section($$) {
    my ($wrap, $name) = @_;
    my $prefix = "Doxy" . $name;
    return
        [ "hash", "Doxy" . $wrap,
          { 
            members =>
              [ "list", $prefix . "s",
                [ "hash", $prefix,
                  {
                    kind => [ "field", $prefix . "Kind" ],
                    name => [ "field", $prefix . "Name" ],
                    virtualness => [ "field", $prefix . "Virtualness" ],
                    protection => [ "field", $prefix . "Protection" ],
                    type => [ "field", $prefix . "Type" ],
                    parameters => 
                      [ "list", $prefix . "Params",
                        [ "hash", $prefix . "Param",
                          {
                            declaration_name => [ "field", $prefix . "ParamName" ],
                            type => [ "field", $prefix . "ParamType" ],
                          },
                        ],
                      ],
                    detailed =>
                      [ "hash", $prefix . "Detailed",
                        {
                          doc => [ "doc", $prefix . "DetailedDoc" ],
                          return => [ "doc", $prefix . "Return" ],
                          see => [ "doc", $prefix . "See" ],
                          params =>
                            [ "list", $prefix . "PDBlocks",
                              [ "hash", $prefix . "PDBlock",
                                {
                                  parameters =>
                                    [ "list", $prefix . "PDParams",
                                      [ "hash", $prefix . "PDParam",
                                        {
                                          name => [ "field", $prefix . "PDParamName" ],
                                        },
                                      ],
                                    ],
                                  doc => [ "doc", $prefix . "PDDoc" ],
                                },
                              ],
                            ],
                        },
                      ],
                  },
                ],
              ],
          },
        ];
}

$doxymodel =
    [ "hash", "DoxyTop",
      {
        files => 
          [ "list", "DoxyFiles",
            [ "hash", "DoxyFile",
              {
                name => [ "field", "DoxyFileName" ],
                typedefs => section("FileTypedefs", "Typedef"),
                variables => section("FileVariables", "Variable"),
                functions => section("FileFunctions", "Function"),
                detailed =>
                  [ "hash", "DoxyFileDetailed",
                    {
                      doc => [ "doc", "DoxyFileDetailedDoc" ],
                    },
                  ],
              },
            ],
          ],
	classes => 
	  [ "list", "DoxyClasses",
	    [ "hash", "DoxyClass",
	      {
		name => [ "field", "DoxyClassName" ],
		public_typedefs => section("ClassPublicTypedefs", "Typedef"),
		public_methods => section("ClassPublicMethods", "Function"),
		public_members => section("ClassPublicMembers", "Variable"),
		protected_typedefs => section("ClassProtectedTypedefs", "Typedef"),
		protected_methods => section("ClassProtectedMethods", "Function"),
		protected_members => section("ClassProtectedMembers", "Variable"),
		private_typedefs => section("ClassPrivateTypedefs", "Typedef"),
		private_methods => section("ClassPrivateMethods", "Function"),
		private_members => section("ClassPrivateMembers", "Variable"),
                detailed =>
		  [ "hash", "DoxyClassDetailed",
		    {
		      doc => [ "doc", "DoxyClassDetailedDoc" ],
 		    },
		  ],
	      },
	    ],
	  ],
      },
    ];

1;
