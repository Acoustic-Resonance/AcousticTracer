# AcousticTracer

Our project 'Acoustic Tracer' is a three-dimensional, acoustic visualiser that allows users to visualise sound travelling throughout a modelled environment as a heat map. At the core it is a C library, where the user can read in a `.glb` file (3D Model File), insert one or more speakers, specify simulation settings, and receive back a heat map of how the sound travelled through the environment over time. Our final project extends it into a web-based application that renders this heat map and makes the configuration of the scene and simulation more user-friendly, while also allowing more technical users to achieve their desired configuration.

Some demonstrations of it's functionality:

<p align="center">
  <img src="assets/clips/bathroom.gif" alt="Bathroom demo" width="46%" height="100%" />
  &nbsp; &nbsp; &nbsp; &nbsp;
  <img src="assets/clips/cathedral.gif" alt="Cathedral demo" width="46%" height="100%" />
</p>

<p align="center">
  <img src="assets/clips/conference.gif" alt="Conference demo" width="45%" height="100%" />
</p>

Project report: [link to the report](docs/report.pdf)</br>
Project website: [link to the documentation](https://acoustic-resonance.github.io/AcousticTracer/)

## Contributors

- [Michael McCarthy](https://github.com/mccarthy-michael)
- [Patryk Mrozek](https://github.com/patrykmrozek)
- [Eoghan Murphy](https://github.com/eonmurph)
- [Alex Wright](https://github.com/alexodwright)

Future Plans:

- [ ] Frontend Multiple Source Selection
- [ ] LiDar 3D Environment Compatability
- [ ] Genetic Algorithm for Optimal Source Position & Orientation
