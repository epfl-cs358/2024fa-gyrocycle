<template>
  <div ref="viewer" class="stl-viewer"></div>
</template>

<script>
import * as THREE from "three";
import { STLLoader } from "three/examples/jsm/loaders/STLLoader.js";
import { TrackballControls } from "three/examples/jsm/controls/TrackballControls.js";

export default {
  name: "StlViewer",
  props: {
    stlUrl: {
      type: String,
      required: true,
    },
    meshColor: {
      type: String,
      default: "#0077be"
    },
    backgroundColor: {
      type: String,
      default: "#000000"
    },
    backgroundOpacity: {
      type: Number,
      default: 0
    },
    initialZoom: {
      type: Number,
      default: 0.6 // Default zoom level (1.0 means no additional zoom applied)
    }
  },
  mounted() {
    this.initThreeJS();
  },
  methods: {
    initThreeJS() {
      // Scene, Camera, Renderer
      const scene = new THREE.Scene();
      const camera = new THREE.PerspectiveCamera(
        75,
        this.$refs.viewer.clientWidth / this.$refs.viewer.clientHeight,
        0.1,
        1000
      );
      camera.position.set(0, 0, 10);

      const renderer = new THREE.WebGLRenderer();
      renderer.setSize(
        this.$refs.viewer.clientWidth,
        this.$refs.viewer.clientHeight
      );
      this.$refs.viewer.appendChild(renderer.domElement);
      renderer.setClearColor(this.backgroundColor, this.backgroundOpacity); // Background color and opacity

      // Add a Hemisphere Light for overall brightness
      const hemisphereLight = new THREE.HemisphereLight(0xffffff, 0x444444, 1.5); // Sky color, ground color, intensity
      hemisphereLight.position.set(0, 20, 0);
      scene.add(hemisphereLight);

      // Add a Directional Light for better shading
      const directionalLight = new THREE.DirectionalLight(0xffffff, 1.2); // White light, intensity
      directionalLight.position.set(10, 10, 10);
      directionalLight.castShadow = true;
      scene.add(directionalLight);

      // Add Ambient Light for soft overall brightness
      const ambientLight = new THREE.AmbientLight(0xffffff, 0.6); // White light, lower intensity
      scene.add(ambientLight);


      // OrbitControls for Interaction
      const controls = new TrackballControls(camera, renderer.domElement);
      controls.rotateSpeed = 5.0; // Adjust rotation speed
      controls.zoomSpeed = 1.2;   // Zoom sensitivity
      controls.panSpeed = 0.8;    // Panning speed

      controls.noZoom = false;    // Allow zooming
      controls.noPan = false;     // Allow panning
      controls.staticMoving = false; // Smooth rotations
      controls.dynamicDampingFactor = 0.2; // Damping factor for smooth motion

      // Load STL File
      const loader = new STLLoader();
      loader.load(
        this.stlUrl,
        (geometry) => {
          const material = new THREE.MeshStandardMaterial({ color: this.meshColor });
          const mesh = new THREE.Mesh(geometry, material);
          scene.add(mesh);
        
          // Center the geometry
          const center = new THREE.Vector3();
          geometry.computeBoundingBox();
          geometry.boundingBox.getCenter(center).negate();
          geometry.translate(center.x, center.y, center.z);
        
          // Fix orientation (rotate from Z-up to Y-up)
          mesh.rotation.set(-Math.PI / 2, 0, 0); // Rotate 90 degrees around X-axis

          // Adjust controls target to the object center
          controls.target.set(0, 0, 0);  // Set target to the center of the model
        
          // Optionally, adjust camera position based on model size
          const size = geometry.boundingBox.getSize(new THREE.Vector3());
          const maxDimension = Math.max(size.x, size.y, size.z);
          const cameraDistance = maxDimension * this.initialZoom;
          camera.position.set(cameraDistance, cameraDistance, cameraDistance); // Position the camera based on size
        
          // Start Rendering
          const animate = () => {
            requestAnimationFrame(animate);
            controls.update(); // Smooth interactions
            renderer.render(scene, camera);
          };
          animate();
        },
        undefined,
        (error) => {
          console.error("An error occurred loading the STL file:", error);
        }
      );
    },
  },
};
</script>

<style scoped>
.stl-viewer {
  width: 100%;
  height: 400px;
  border: 1px solid #ddd;
  overflow: hidden;
}
</style>
