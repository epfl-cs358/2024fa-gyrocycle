<template>
    <div class="warning custom-block" v-if="!showPreviews">
      <p class="custom-block-title">No preview on smaller screens</p>
      <p>STL file previews are not available on smaller screens, please use a laptop or increase the size of your browser window.</p>
    </div>
    <table>
        <thead>
          <tr>
            <th v-if="showPreviews">Preview</th>
            <th>File</th>
            <th>Qty</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(file, index) in stlFiles" :key="index">
            <td v-if="showPreviews" class="preview-column">
                <StlViewer :stlUrl="file.url" meshColor="#a16207" class="stl-preview" />
            </td>
            <td class="file-column">
              <a :href="file.dxf" download>{{ file.dxfName }}</a>
            </td>
            <td class="qty-column">{{ file.quantity }}</td>
          </tr>
        </tbody>
    </table>
    </template>
    
    <script>
    import StlViewer from "./StlViewer.vue"; // Import your STL Viewer component
  
    const additionalProperties = {
        "bicycle-mdf-bottom.stl": {
            quantity: 1
        },
        "bicycle-mdf-flywheel-holder.stl": {
            quantity: 1
        },
        "bicycle-mdf-flywheel-motor-plate.stl": {
            quantity: 1
        },
        "bicycle-mdf-front-wheel-fork-holder.stl": {
            quantity: 1
        },
        "bicycle-mdf-lipo-protection-circuit-support.stl": {
            quantity: 1
        },
        "bicycle-mdf-servo-holder.stl": {
            quantity: 1
        },
        "bicycle-mdf-shaft-holder.stl": {
            quantity: 1
        },
        "bicycle-mdf-side.stl": {
            quantity: 2
        },
        "bicycle-mdf-top.stl": {
            quantity: 1
        },
    }

    export default {
      name: "BicyclePartsToLaserCutTable",
      components: {
        StlViewer,
      },
      data() {
        return {
          stlFiles: [],
          showPreviews: true
        };
      },
      created() {
        // Dynamically fetch all STL files from the public/stl folder
        const files = import.meta.glob("/public/stl/to-be-laser-cut/bicycle/*.stl", { eager: true });
        console.log(`${Object.keys(files).length} files loaded`);
    
        // Process the files into a usable format
        this.stlFiles = Object.keys(files).map((filePath) => {
          let filename = filePath.split("/").pop()
          let productionPath = filePath.replace("/public", "");
          return {
            name: filename, // Extract file name
            url: productionPath, // Generate accessible URL
            dxfName: filename.replace("/stl/to-be-laser-cut/bicycle/", "/dxf/").replace(".stl", ".dxf"),
            dxf: productionPath.replace("/stl/to-be-laser-cut/bicycle/", "/dxf/").replace(".stl", ".dxf"),
            ...additionalProperties[filename]
          };
        });
  
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