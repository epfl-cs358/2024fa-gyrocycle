<template>
  <div class="warning custom-block" v-if="!showPreviews">
    <p class="custom-block-title">No preview on smaller screens</p>
    <p>STL file previews are not available on smaller screens, please use a laptop or increase the size of your browser window.</p>
  </div>
  <div v-for="(files, group) in groupedFiles" :key="group">
    <h2 :id="group" tabindex="-1">{{ groupTitles[group] }}<a class="header-anchor" :href="`#${group}`" :aria-label="`Permalink to &quot;${groupTitles[group]}&quot;`">​</a></h2>
    <table>
        <thead>
          <tr>
            <th v-if="showPreviews">Preview</th>
            <th>File</th>
            <th>Qty</th>
            <th>Material</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(file, index) in files" :key="index">
            <td v-if="showPreviews" class="preview-column">
                <StlViewer :stlUrl="file.url" class="stl-preview" />
            </td>
            <td class="file-column">
              <a :href="file.url" download>{{ file.name }}</a>
            </td>
            <td class="qty-column">{{ file.quantity }}</td>
            <td class="material-column">{{ file.material }}</td>
          </tr>
        </tbody>
      </table>
  </div>
  </template>
  
  <script>
  import StlViewer from "./StlViewer.vue"; // Import your STL Viewer component

  const additionalProperties = {
    // Child Wheels
    "child-wheel-shaft.stl": {
      quantity: 2,
      material: "PETG",
      group: "child-wheels"
    },
    "child-wheels-support.stl": {
      quantity: 1,
      material: "PETG",
      group: "child-wheels"
    },

    // Electronics
    "buck-converter-screw-spacer.stl": {
      quantity: 2,
      material: "PETG",
      group: "electronics"
    },
    "esp32-screw-spacer.stl": {
      quantity: 4,
      material: "PETG",
      group: "electronics"
    },
    "odrive-plate.stl": {
      quantity: 1,
      material: "PETG",
      group: "electronics"
    },
    "optical-sensor-holder.stl": {
      quantity: 2,
      material: "PETG",
      group: "electronics"
    },
    "propulsion-driver-plate.stl": {
      quantity: 1,
      material: "PETG",
      group: "electronics"
    },
    "protection-board-holder.stl": {
      quantity: 2,
      material: "PETG",
      group: "electronics"
    },

    // Flywheel
    "flywheel.stl": {
      quantity: 1,
      material: "PETG",
      group: "flywheel"
    },

    // Front wheel
    "front-wheel-fork.stl": {
      quantity: 1,
      material: "PETG",
      group: "front-wheel"
    },
    "front-wheel-rim.stl": {
      quantity: 1,
      material: "PETG",
      group: "front-wheel"
    },
    "front-wheel-shaft.stl": {
      quantity: 1,
      material: "PETG",
      group: "front-wheel"
    },
    "front-wheel-spacer.stl": {
      quantity: 2,
      material: "PETG",
      group: "front-wheel"
    },
    "front-wheel-tire.stl": {
      quantity: 1,
      material: "TPU",
      group: "front-wheel"
    },
    "servo-large-gear.stl": {
      quantity: 1,
      material: "PETG",
      group: "front-wheel"
    },
    "servo-small-gear.stl": {
      quantity: 1,
      material: "PETG",
      group: "front-wheel"
    },

    // LIPO Protection Board
    "lipo-protection-board-bottom.stl": {
      quantity: 1,
      material: "PETG",
      group: "lipo-protection"
    },
    "lipo-protection-board-box.stl": {
      quantity: 1,
      material: "PETG",
      group: "lipo-protection"
    },
    
    // Rear Wheel
    "rear-wheel-rim.stl": {
      quantity: 1,
      material: "PETG",
      group: "rear-wheel"
    },
    "rear-wheel-shaft.stl": {
      quantity: 1,
      material: "PETG",
      group: "rear-wheel"
    },
    "rear-wheel-spacer.stl": {
      quantity: 2,
      material: "PETG",
      group: "rear-wheel"
    },
    "rear-wheel-tire.stl": {
      quantity: 1,
      material: "TPU",
      group: "rear-wheel"
    },
  }

  export default {
    name: "ComponentsToPrintTable",
    components: {
      StlViewer,
    },
    data() {
      return {
        stlFiles: [],
        showPreviews: true,
        groupTitles: {
          "child-wheels": "Child Wheels",
          "electronics": "Electronics and Supports",
          "flywheel": "Flywheel",
          "front-wheel": "Front Wheel",
          "lipo-protection": "LIPO Protection Board",
          "rear-wheel": "Rear Wheel"
        },
        groupedFiles: {}
      };
    },
    created() {
      // Dynamically fetch all STL files from the public/stl folder
      const files = import.meta.glob("/public/stl/to-be-printed/*.stl", { eager: true });
      console.log(files);
  
      // Process the files into a usable format
      this.stlFiles = Object.keys(files).map((filePath) => {
        let productionPath = filePath.replace("/public", "");
        let filename = productionPath.split("/").pop()
        return {
          name: filename, // Extract file name
          url: productionPath, // Generate accessible URL
          ...additionalProperties[filename]
        };
      });

      // Group the stlFiles based on their group property
      this.groupedFiles = this.stlFiles.reduce((acc, file) => {
        if (!acc[file.group]) {
          acc[file.group] = [];
        }
        acc[file.group].push(file);
        return acc;
      }, {});

      // Handle resizing
      this.handleResize();
      window.addEventListener("resize", this.handleResize);
    },
    methods: {
      handleResize() {
        // Update showPreviews based on window width
        this.showPreviews = window.innerWidth >= 705;
      }
    },
    beforeDestroy() {
      window.removeEventListener("resize", this.handleResize);
    }
  };
  </script>
  
  <style scoped>
.table-container {
  width: 100%; /* Ensure the container spans full width */
  overflow-x: auto; /* Handle overflow in case of small screens */
}

table {
  width: 100%;
  table-layout: fixed; /* Enforces fixed layout */
  border-collapse: collapse;
}

th,
td {
  padding: 8px;
  border: 1px solid #ddd; /* Optional for visual borders */
  text-align: left;
}

thead {
  background-color: #f4f4f4; /* Optional header background */
}

.preview-column {
  width: 1%; /* Start with minimal space, then grow */
}

.preview-column,
.stl-preview {
  width: 90%;
  height: 150px; /* Adjust as needed */
  object-fit: contain; /* Ensures the STL preview fits nicely */
}

.file-column,
.qty-column,
.material-column {
  white-space: nowrap; /* Prevent columns from wrapping text */
  width: auto; /* Take up as much space as needed */
}

tbody tr {
  height: 150px; /* Ensure rows have enough height for the preview */
}
</style>