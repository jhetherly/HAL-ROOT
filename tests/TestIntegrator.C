TestIntegrator()
{
  gSystem->Load("libHAL");

  TF1 f1("", "1.0/sqrt(x)", 0, 1);
  TF1 f2("", "pow(x, 2)*sin(1.0/x)", -1, 2);
  TF1 f3("", "exp(-1.0*pow(x-1.0, 2))", -1000, 1000);
  TF1 f4("", "cos(sin(x)/pow(x, 1.2))/pow(x, .3)", 0, 1);
  HAL::Integrator integrator;
  Double_t f1_root, f1_hal, f2_root, f2_hal,
           f3_root, f3_hal, f4_root, f4_hal;

  f1_root = f1.Integral(0, 1);
  f1_hal = integrator.Integrate(f1, 0, 1);
  f2_root = f2.Integral(-1, 2);
  f2_hal = integrator.Integrate(f2, -1, 2);
  f3_root = f3.Integral(-1000, 1000);
  f3_hal = integrator.Integrate(f3, -1000, 1000);
  f4_root = f4.Integral(0, 1);
  f4_hal = integrator.Integrate(f4, 0, 1);

  std::cout << std::endl;
  std::cout << "Integral" << "\t\t\t\t\tROOT" << "\t\tHAL" <<
    "\t\tActual Value" << std::endl;
  std::cout << "pow(x, 2)*sin(1.0/x) from -1 to 2:\t\t" << 
    f2_root << "\t\t" << f2_hal << "\t\t" << 1.38755 << std::endl;
  std::cout << "exp(-1.0*pow(x-1.0, 2)) from -1000 to 1000:\t" << 
    f3_root << "\t\t" << f3_hal << "\t\t" << 1.77245 << std::endl;
  std::cout << "cos(sin(x)/pow(x, 1.2))/pow(x, .3) from 0 to 1:\t" << 
    f4_root << "\t" << f4_hal << "\t" << 0.358465 << std::endl;
  std::cout << "1/sqrt(x) from 0 to 1:\t\t\t\t" << f1_root <<
    "\t" << f1_hal << "\t\t" << 2 << std::endl;
}
