import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "GyroCycle",
  description: "Self-balancing bicycle.",
  markdown: {
    math: true
  },
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    nav: [
      //{ text: 'Home', link: '/' }
    ],

    sidebar: [
      //{
      //  text: 'Examples',
      //  items: [
      //    { text: 'Markdown Examples', link: '/markdown-examples' },
      //    { text: 'Runtime API Examples', link: '/api-examples' }
      //  ]
      //}
    ],

    socialLinks: [
      { icon: 'github', link: 'https://github.com/epfl-cs358/2024fa-gyrocycle' }
    ]
  }
})
