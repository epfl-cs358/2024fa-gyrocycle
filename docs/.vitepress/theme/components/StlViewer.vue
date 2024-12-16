<template>
  <div ref="viewer" class="stl-viewer"></div>
</template>

<script>
import * as THREE from "three";
import { STLLoader } from "three/examples/jsm/loaders/STLLoader.js";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls.js";

export default {
  name: "StlViewer",
  props: {
    stlUrl: {
      type: String,
      required: true,
    },
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
      const controls = new OrbitControls(camera, renderer.domElement);
      controls.enableDamping = true; // Smooth rotation
      controls.dampingFactor = 0.05;

      // Load STL File
      const loader = new STLLoader();
      loader.load(
        this.stlUrl,
        (geometry) => {
          const material = new THREE.MeshStandardMaterial({ color: 0x0077be });
          const mesh = new THREE.Mesh(geometry, material);
          scene.add(mesh);
        
          // Center the geometry
          const center = new THREE.Vector3();
          geometry.computeBoundingBox();
          geometry.boundingBox.getCenter(center).negate();
          geometry.translate(center.x, center.y, center.z);
        
          // Adjust controls target to the object center
          controls.target.set(0, 0, 0);  // Set target to the center of the model
          controls.update();  // Update the controls after modifying the target
        
          // Optionally, adjust camera position based on model size
          const size = geometry.boundingBox.getSize(new THREE.Vector3());
          const maxDimension = Math.max(size.x, size.y, size.z);
          const cameraDistance = maxDimension * 0.4;
          camera.position.set(cameraDistance, cameraDistance, cameraDistance); // Position the camera based on size
        
          // Rotate the mesh so it is displayed horizontally, vertically, or diagonally
          mesh.rotation.x = Math.PI / 2; // Example: Rotate 90 degrees along the X axis (horizontal)
          // mesh.rotation.y = Math.PI / 2; // Example: Rotate 90 degrees along the Y axis (vertical)
          // mesh.rotation.z = Math.PI / 2; // Example: Rotate 90 degrees along the Z axis (diagonal)
        
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
