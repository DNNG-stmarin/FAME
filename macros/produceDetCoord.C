{
	double phiAll[3] = {0, 35, 65};

	double theta, phi;

	double degToRad = TMath::Pi()/180;

	double x,y,z;

	ofstream out;
  	string fileout = "cartesianDirs.txt";
  	out.open(fileout);

	for(int ref = 0; ref >= -1; ref --)
	{
		for(int arc = 0; arc < 3; arc++)
		{
			for(int det = 0; det < 9; det++)
			{
				theta = 150 - det*15;

				if(ref == 0)
				{
					phi = phiAll[arc];
				}

				if(ref == -1)
				{
					phi = 180 - phiAll[arc];
				}

				theta *= degToRad;
				phi *= degToRad;

				x = sin(theta)*cos(phi);
				y = sin(theta)*sin(phi);
				z = cos(theta);

				out << x << " " << y << " " << z << endl;


			}
		}
	}
}
