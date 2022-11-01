using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Identity;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;

namespace Taxi
{
    public class Program
    {
        public static void Main(string[] args)
        {
            IHost host = CreateHostBuilder(args)
                         .Build();

            using (var scope = host.Services.CreateScope())
            {
                var services = scope.ServiceProvider;
                RoleManager<IdentityRole> manager = services.GetService<RoleManager<IdentityRole>>();

                if (!manager.RoleExistsAsync("Admin").Result)
                {
                  var result = manager.CreateAsync(new IdentityRole("Admin")).Result;
                }

                if (!manager.RoleExistsAsync("Utilizator").Result)
                {
                    var result = manager.CreateAsync(new IdentityRole("Utilizator")).Result;
                }

                if (!manager.RoleExistsAsync("Sofer").Result)
                {
                    var result = manager.CreateAsync(new IdentityRole("Sofer")).Result;
                }

                UserManager<IdentityUser> userManager = services.GetService<UserManager<IdentityUser>>();

         
                var sofer = userManager.FindByEmailAsync("sofer@yahoo.com").Result;
                if (sofer != null)
                {
                    if (!userManager.IsInRoleAsync(sofer, "Sofer").Result)
                        userManager.AddToRoleAsync(sofer, "Sofer");
                }

                host.Run();
            }
        }

        public static IHostBuilder CreateHostBuilder(string[] args) =>
                        Host.CreateDefaultBuilder(args)
                            .ConfigureWebHostDefaults(webBuilder =>
                            {

                                webBuilder.UseStartup<Startup>();
                            });
    }
    
}
