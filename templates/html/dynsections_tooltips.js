// Simple tooltip implementation without jQuery or PowerTip
document.addEventListener('DOMContentLoaded', function() {
  // Create tooltip container
  const tooltip = document.createElement('div');
  tooltip.id = 'powerTip';
  tooltip.style.position = 'absolute';
  tooltip.style.display = 'none';
  tooltip.style.zIndex = '9999';
  document.body.appendChild(tooltip);

  let currentElement = null;
  let hideTimeout = null;

  function showTooltip(element, content) {
    if (!content) return;
    
    clearTimeout(hideTimeout);
    currentElement = element;
    
    tooltip.innerHTML = content;
    tooltip.style.display = 'block';
    
    // Position tooltip below the element
    const rect = element.getBoundingClientRect();
    const tooltipRect = tooltip.getBoundingClientRect();
    
    let left = rect.left + (rect.width - tooltipRect.width) / 2;
    let top = rect.bottom + 10;
    
    // Check if tooltip goes off screen
    if (left < 0) left = 10;
    if (left + tooltipRect.width > window.innerWidth) {
      left = window.innerWidth - tooltipRect.width - 10;
    }
    
    // If tooltip would go below viewport, show above element instead
    if (top + tooltipRect.height > window.innerHeight) {
      top = rect.top - tooltipRect.height - 10;
    }
    
    tooltip.style.left = left + window.scrollX + 'px';
    tooltip.style.top = top + window.scrollY + 'px';
  }

  function hideTooltip() {
    hideTimeout = setTimeout(() => {
      tooltip.style.display = 'none';
      currentElement = null;
    }, 100);
  }

  // Add hover listeners to code elements
  document.querySelectorAll('.code,.codeRef').forEach(element => {
    const href = element.getAttribute('href');
    if (!href) return;
    
    // Get tooltip content from data attribute or related element
    const targetId = 'a' + href.replace(/.*\//, '').replace(/[^a-z_A-Z0-9]/g, '_');
    const contentElement = document.getElementById(targetId);
    const content = contentElement ? contentElement.innerHTML : null;
    
    if (!content) return;
    
    element.addEventListener('mouseenter', function() {
      showTooltip(element, content);
    });
    
    element.addEventListener('mouseleave', function() {
      hideTooltip();
    });
    
    element.addEventListener('focus', function() {
      showTooltip(element, content);
    });
    
    element.addEventListener('blur', function() {
      hideTooltip();
    });
  });

  // Allow mouse to enter tooltip without hiding it
  tooltip.addEventListener('mouseenter', function() {
    clearTimeout(hideTimeout);
  });

  tooltip.addEventListener('mouseleave', function() {
    hideTooltip();
  });
});
